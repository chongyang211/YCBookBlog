// pkg/http/server.go - HTTP/1.1 Server + Mux 路由 + 中间件
//
// 第 5 次会话 Step 11.3-11.4
//
// 架构:
//   net.Listener (由 pkg/mux 或 标准库 提供)
//     → 每连接 goroutine
//       → bufio.Reader 读 Request
//       → Mux 查路由
//       → Handler 填 ResponseWriter
//       → 回写 Response
//       → Keep-Alive 时继续读下一个 Request (阶段 ⑫ 启用)
package http

import (
	"bufio"
	"fmt"
	"net"
	"strings"
	"sync"
	"time"

	"mininet/pkg/common"
)

// ResponseWriter 和 Go 标准库 http.ResponseWriter 接口类似
type ResponseWriter interface {
	Header() Header
	WriteHeader(statusCode int)
	Write(body []byte) (int, error)
}

// HandlerFunc 处理一个请求
type HandlerFunc func(w ResponseWriter, r *Request)

// Mux 简单的路由表 (精确匹配 + 前缀匹配 fallback)
type Mux struct {
	mu       sync.RWMutex
	exact    map[string]HandlerFunc // "GET /foo"
	prefix   []prefixRule
	fallback HandlerFunc
}

type prefixRule struct {
	method  string
	prefix  string
	handler HandlerFunc
}

func NewMux() *Mux {
	return &Mux{
		exact:    make(map[string]HandlerFunc),
		fallback: defaultNotFound,
	}
}

// Handle 精确路径
func (m *Mux) Handle(method, path string, h HandlerFunc) {
	m.mu.Lock()
	m.exact[method+" "+path] = h
	m.mu.Unlock()
}

// HandlePrefix 前缀匹配 (用于静态文件、API 聚合)
func (m *Mux) HandlePrefix(method, prefix string, h HandlerFunc) {
	m.mu.Lock()
	m.prefix = append(m.prefix, prefixRule{method, prefix, h})
	m.mu.Unlock()
}

// SetNotFound 自定义 404
func (m *Mux) SetNotFound(h HandlerFunc) {
	m.mu.Lock()
	m.fallback = h
	m.mu.Unlock()
}

func (m *Mux) Lookup(method, path string) HandlerFunc {
	m.mu.RLock()
	defer m.mu.RUnlock()
	if h, ok := m.exact[method+" "+path]; ok {
		return h
	}
	for _, r := range m.prefix {
		if r.method == method && strings.HasPrefix(path, r.prefix) {
			return r.handler
		}
	}
	return m.fallback
}

func defaultNotFound(w ResponseWriter, r *Request) {
	w.WriteHeader(404)
	w.Header().Set("Content-Type", "text/plain")
	w.Write([]byte("404 not found: " + r.URL + "\n"))
}

// -------- Server --------

type Server struct {
	Addr string
	Mux  *Mux
	// 阶段 ⑫ 新增 Keep-Alive 配置
	KeepAlive       bool          // 默认 false (兼容旧测试);NewServer 显式开启
	IdleTimeout     time.Duration // 空闲超过此时间断开 (默认 30s)
	MaxKeepRequests int           // 单连接最多复用请求数 (默认 100,防止慢连接独占)
	ln              net.Listener
	mu              sync.Mutex
	run             bool
}

func NewServer(addr string, mux *Mux) *Server {
	return &Server{Addr: addr, Mux: mux}
}

// Serve 阻塞运行;Shutdown 后返回 nil
// 外部可传已经 Listen 好的 ln (给 pkg/mux Reactor 用)
func (s *Server) Serve(ln net.Listener) error {
	s.mu.Lock()
	s.ln = ln
	s.run = true
	s.mu.Unlock()
	for {
		conn, err := ln.Accept()
		if err != nil {
			s.mu.Lock()
			running := s.run
			s.mu.Unlock()
			if !running {
				return nil
			}
			return err
		}
		go s.handleConn(conn)
	}
}

// ListenAndServe 便捷方法
func (s *Server) ListenAndServe() error {
	ln, err := net.Listen("tcp", s.Addr)
	if err != nil {
		return err
	}
	return s.Serve(ln)
}

func (s *Server) Shutdown() error {
	s.mu.Lock()
	s.run = false
	ln := s.ln
	s.mu.Unlock()
	if ln != nil {
		return ln.Close()
	}
	return nil
}

func (s *Server) handleConn(c net.Conn) {
	defer c.Close()
	br := bufio.NewReader(c)

	idle := s.IdleTimeout
	if idle <= 0 {
		idle = 30 * time.Second
	}
	maxReq := s.MaxKeepRequests
	if maxReq <= 0 {
		maxReq = 100
	}

	// 阶段 ⑫ Keep-Alive 循环: 同一连接处理多个请求
	for i := 0; ; i++ {
		c.SetReadDeadline(time.Now().Add(idle))
		req, err := ReadRequest(br)
		if err != nil {
			common.Trace("http", "read req: %v", err)
			return
		}
		common.Info("http", "%s %s %s (req#%d on conn)", req.Method, req.URL, req.Proto, i+1)

		// 路由 + 执行
		h := s.Mux.Lookup(req.Method, req.URL)
		rw := &respWriter{header: make(Header), code: 200}
		h(rw, req)

		// 决策是否保持连接 (RFC7230 §6.3)
		//   HTTP/1.1 默认 Keep-Alive, 除非 "Connection: close"
		//   HTTP/1.0 默认 close, 除非 "Connection: keep-alive"
		keep := s.KeepAlive && i+1 < maxReq && shouldKeepAlive(req, rw)

		resp := &Response{
			Proto:      "HTTP/1.1",
			StatusCode: rw.code,
			Header:     rw.header,
			Body:       rw.body,
		}
		if keep {
			resp.Header.Set("Connection", "keep-alive")
			resp.Header.Set("Keep-Alive", fmt.Sprintf("timeout=%d, max=%d",
				int(idle.Seconds()), maxReq-i-1))
		} else {
			resp.Header.Set("Connection", "close")
		}
		if resp.Header.Get("Server") == "" {
			resp.Header.Set("Server", "mnet/0.6")
		}
		c.SetWriteDeadline(time.Now().Add(10 * time.Second))
		if _, err := resp.WriteTo(c); err != nil {
			return
		}
		if !keep {
			return
		}
	}
}

func shouldKeepAlive(req *Request, rw *respWriter) bool {
	// 客户端明确要求关
	if strings.EqualFold(req.Header.Get("Connection"), "close") {
		return false
	}
	// handler 明确设置关
	if strings.EqualFold(rw.header.Get("Connection"), "close") {
		return false
	}
	// HTTP/1.0 默认关,除非显式 keep-alive
	if req.Proto == "HTTP/1.0" {
		return strings.EqualFold(req.Header.Get("Connection"), "keep-alive")
	}
	return true // HTTP/1.1 默认开
}

// respWriter 内部 ResponseWriter 实现
type respWriter struct {
	header     Header
	code       int
	body       []byte
	wroteHeader bool
}

func (w *respWriter) Header() Header { return w.header }

func (w *respWriter) WriteHeader(code int) {
	w.code = code
	w.wroteHeader = true
}

func (w *respWriter) Write(b []byte) (int, error) {
	w.body = append(w.body, b...)
	return len(b), nil
}

// -------- 便捷帮手 --------

// WriteString 便捷:发文本
func WriteString(w ResponseWriter, code int, s string) {
	w.WriteHeader(code)
	if w.Header().Get("Content-Type") == "" {
		w.Header().Set("Content-Type", "text/plain; charset=utf-8")
	}
	w.Write([]byte(s))
}

// WriteJSON 便捷:发 JSON (简化,只接 string)
func WriteJSON(w ResponseWriter, code int, s string) {
	w.WriteHeader(code)
	w.Header().Set("Content-Type", "application/json")
	w.Write([]byte(s))
}

// Redirect 302 跳转
func Redirect(w ResponseWriter, code int, location string) {
	w.Header().Set("Location", location)
	w.WriteHeader(code)
	w.Write(fmt.Appendf(nil, "redirect to %s\n", location))
}
