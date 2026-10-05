// pkg/http/message.go - HTTP/1.1 请求 / 响应结构 + 编解码
//
// 第 5 次会话 Step 11.1-11.2
//
// HTTP/1.1 是文本协议 (RFC7230),格式:
//
//   Request:
//     GET /path HTTP/1.1\r\n
//     Host: example.com\r\n
//     Content-Length: 5\r\n
//     \r\n
//     hello
//
//   Response:
//     HTTP/1.1 200 OK\r\n
//     Content-Type: text/html\r\n
//     Content-Length: 11\r\n
//     \r\n
//     Hello World
//
// 本模块自己写解析器 (不用 net/http.ReadRequest),目的是看懂每一个字节.
package http

import (
	"bufio"
	"bytes"
	"errors"
	"fmt"
	"io"
	"strconv"
	"strings"
)

const (
	MaxHeaderBytes = 1 << 20 // 1 MB,防止 header 攻击
	CRLF           = "\r\n"
)

// Header 大小写不敏感的头部 (Go 标准库是 map[string][]string,我们偷懒用单值)
type Header map[string]string

// Get 大小写不敏感查找
func (h Header) Get(k string) string {
	for key, v := range h {
		if strings.EqualFold(key, k) {
			return v
		}
	}
	return ""
}

// Set
func (h Header) Set(k, v string) { h[k] = v }

// Request HTTP 请求
type Request struct {
	Method  string            // GET/POST/...
	URL     string            // /path?query
	Proto   string            // HTTP/1.1
	Header  Header
	Body    []byte
	// 阶段 ⑫ 预留:Host 从 Header 抽出
}

// Response HTTP 响应
type Response struct {
	Proto      string
	StatusCode int
	Status     string // "200 OK"
	Header     Header
	Body       []byte
}

// -------- 解析 Request --------

// ReadRequest 从 bufio.Reader 读一个完整请求 (header + body)
// 遵守 Content-Length / Transfer-Encoding: chunked
func ReadRequest(r *bufio.Reader) (*Request, error) {
	// 1. 读 request line: "GET /path HTTP/1.1\r\n"
	line, err := readLine(r)
	if err != nil {
		return nil, err
	}
	parts := strings.SplitN(line, " ", 3)
	if len(parts) != 3 {
		return nil, fmt.Errorf("bad request line: %q", line)
	}
	req := &Request{
		Method: parts[0],
		URL:    parts[1],
		Proto:  parts[2],
		Header: make(Header),
	}

	// 2. 读 header 直到空行
	if err := readHeaders(r, req.Header); err != nil {
		return nil, err
	}

	// 3. 读 body (如有)
	body, err := readBody(r, req.Header)
	if err != nil {
		return nil, err
	}
	req.Body = body
	return req, nil
}

// ReadResponse 从 bufio.Reader 读一个完整响应
func ReadResponse(r *bufio.Reader) (*Response, error) {
	line, err := readLine(r)
	if err != nil {
		return nil, err
	}
	// "HTTP/1.1 200 OK"
	parts := strings.SplitN(line, " ", 3)
	if len(parts) < 2 {
		return nil, fmt.Errorf("bad status line: %q", line)
	}
	resp := &Response{
		Proto:  parts[0],
		Header: make(Header),
	}
	code, err := strconv.Atoi(parts[1])
	if err != nil {
		return nil, fmt.Errorf("bad status code: %q", parts[1])
	}
	resp.StatusCode = code
	if len(parts) == 3 {
		resp.Status = parts[1] + " " + parts[2]
	} else {
		resp.Status = parts[1]
	}

	if err := readHeaders(r, resp.Header); err != nil {
		return nil, err
	}
	body, err := readBody(r, resp.Header)
	if err != nil {
		return nil, err
	}
	resp.Body = body
	return resp, nil
}

// -------- 编码 --------

// WriteTo 把 Request 写入 w (客户端用)
func (req *Request) WriteTo(w io.Writer) (int64, error) {
	var buf bytes.Buffer
	fmt.Fprintf(&buf, "%s %s %s%s", req.Method, req.URL, req.Proto, CRLF)
	// Host 必须有
	if req.Header.Get("Host") == "" {
		return 0, errors.New("request missing Host header")
	}
	// Content-Length 自动设置
	if len(req.Body) > 0 && req.Header.Get("Content-Length") == "" {
		req.Header.Set("Content-Length", strconv.Itoa(len(req.Body)))
	}
	for k, v := range req.Header {
		fmt.Fprintf(&buf, "%s: %s%s", k, v, CRLF)
	}
	buf.WriteString(CRLF)
	buf.Write(req.Body)
	n, err := w.Write(buf.Bytes())
	return int64(n), err
}

// WriteTo 把 Response 写入 w (服务端用)
func (resp *Response) WriteTo(w io.Writer) (int64, error) {
	var buf bytes.Buffer
	status := resp.Status
	if status == "" {
		status = fmt.Sprintf("%d %s", resp.StatusCode, StatusText(resp.StatusCode))
	}
	fmt.Fprintf(&buf, "%s %s%s", resp.Proto, status, CRLF)
	if resp.Header == nil {
		resp.Header = make(Header)
	}
	if resp.Header.Get("Content-Length") == "" && resp.Header.Get("Transfer-Encoding") == "" {
		resp.Header.Set("Content-Length", strconv.Itoa(len(resp.Body)))
	}
	for k, v := range resp.Header {
		fmt.Fprintf(&buf, "%s: %s%s", k, v, CRLF)
	}
	buf.WriteString(CRLF)
	buf.Write(resp.Body)
	n, err := w.Write(buf.Bytes())
	return int64(n), err
}

// -------- 辅助 --------

func readLine(r *bufio.Reader) (string, error) {
	line, err := r.ReadString('\n')
	if err != nil {
		return "", err
	}
	// strip trailing \r\n
	line = strings.TrimRight(line, "\r\n")
	return line, nil
}

func readHeaders(r *bufio.Reader, h Header) error {
	totalBytes := 0
	for {
		line, err := readLine(r)
		if err != nil {
			return err
		}
		totalBytes += len(line) + 2
		if totalBytes > MaxHeaderBytes {
			return fmt.Errorf("header too large: %d bytes", totalBytes)
		}
		if line == "" {
			return nil // 空行结束
		}
		// "Key: Value"
		colonIdx := strings.Index(line, ":")
		if colonIdx < 0 {
			return fmt.Errorf("bad header line: %q", line)
		}
		k := line[:colonIdx]
		v := strings.TrimSpace(line[colonIdx+1:])
		h[k] = v
	}
}

func readBody(r *bufio.Reader, h Header) ([]byte, error) {
	// chunked 优先 (RFC7230 §3.3.3)
	if strings.EqualFold(h.Get("Transfer-Encoding"), "chunked") {
		return readChunked(r)
	}
	cl := h.Get("Content-Length")
	if cl == "" {
		return nil, nil
	}
	n, err := strconv.Atoi(cl)
	if err != nil {
		return nil, fmt.Errorf("bad Content-Length: %q", cl)
	}
	if n == 0 {
		return nil, nil
	}
	buf := make([]byte, n)
	if _, err := io.ReadFull(r, buf); err != nil {
		return nil, fmt.Errorf("read body: %w", err)
	}
	return buf, nil
}

// readChunked RFC7230 §4.1
//
//   chunk-size (hex) CRLF
//   chunk-data CRLF
//   ...
//   0 CRLF
//   trailer-headers CRLF (可选)
//   CRLF
func readChunked(r *bufio.Reader) ([]byte, error) {
	var body bytes.Buffer
	for {
		sizeLine, err := readLine(r)
		if err != nil {
			return nil, err
		}
		// 可能带 chunk-ext (";" 后缀),去掉
		if idx := strings.Index(sizeLine, ";"); idx >= 0 {
			sizeLine = sizeLine[:idx]
		}
		size, err := strconv.ParseInt(strings.TrimSpace(sizeLine), 16, 64)
		if err != nil {
			return nil, fmt.Errorf("bad chunk size %q: %w", sizeLine, err)
		}
		if size == 0 {
			// 读完 trailer (本案例跳过,只吃一个空行)
			if _, err := readLine(r); err != nil && err != io.EOF {
				return nil, err
			}
			return body.Bytes(), nil
		}
		chunk := make([]byte, size)
		if _, err := io.ReadFull(r, chunk); err != nil {
			return nil, err
		}
		body.Write(chunk)
		// 吃掉 chunk 后的 CRLF
		if _, err := readLine(r); err != nil {
			return nil, err
		}
	}
}

// StatusText 常见 HTTP 状态码 → 文本
func StatusText(code int) string {
	switch code {
	case 100:
		return "Continue"
	case 200:
		return "OK"
	case 201:
		return "Created"
	case 204:
		return "No Content"
	case 301:
		return "Moved Permanently"
	case 302:
		return "Found"
	case 304:
		return "Not Modified"
	case 400:
		return "Bad Request"
	case 401:
		return "Unauthorized"
	case 403:
		return "Forbidden"
	case 404:
		return "Not Found"
	case 500:
		return "Internal Server Error"
	case 502:
		return "Bad Gateway"
	case 503:
		return "Service Unavailable"
	}
	return "Unknown"
}
