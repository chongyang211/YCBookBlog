// pkg/http/pool.go - HTTP Client 连接池 (Keep-Alive 复用)
//
// 第 6 次会话 Step 12.3
//
// 为什么要连接池?
//   每次 HTTP 请求都新开 TCP → 3 次握手 1 RTT + TLS 1.3 1 RTT = 2 RTT 浪费
//   带 KA 池后: 第 1 次 2 RTT, 后续 0 RTT 直接发请求
//
// 设计:
//   map[host] → chan net.Conn (有缓冲 chan 做池)
//   Get: 从 chan 非阻塞取 → 没有就新建
//   Put: 塞回 chan → 满了就关
package http

import (
	"net"
	"sync"
	"time"

	"mininet/pkg/common"
)

// ConnPool 按 host 分桶的 TCP 连接池
type ConnPool struct {
	mu     sync.Mutex
	pools  map[string]chan *pooledConn
	maxPer int           // 每 host 最多缓存几个
	maxIdle time.Duration // 空闲多久自动关
}

type pooledConn struct {
	net.Conn
	lastUsed time.Time
}

// NewConnPool
func NewConnPool(maxPerHost int, maxIdle time.Duration) *ConnPool {
	if maxPerHost <= 0 {
		maxPerHost = 4
	}
	if maxIdle <= 0 {
		maxIdle = 90 * time.Second
	}
	return &ConnPool{
		pools:   make(map[string]chan *pooledConn),
		maxPer:  maxPerHost,
		maxIdle: maxIdle,
	}
}

// Get 从池里拿一个连接;没有就 dialFn 新建
// 返回 (conn, isReused)
func (p *ConnPool) Get(host string, dialFn func(string) (net.Conn, error)) (net.Conn, bool, error) {
	ch := p.getPool(host)
	// 非阻塞拿
	for {
		select {
		case pc := <-ch:
			// 过期的直接丢
			if time.Since(pc.lastUsed) > p.maxIdle {
				pc.Close()
				continue
			}
			common.Trace("http", "reuse conn to %s", host)
			return pc.Conn, true, nil
		default:
			// 池空,新建
			c, err := dialFn(host)
			if err != nil {
				return nil, false, err
			}
			common.Trace("http", "new conn to %s", host)
			return c, false, nil
		}
	}
}

// Put 把用完的连接塞回池
// 如果 shouldClose=true (如 Connection: close) 就直接关
func (p *ConnPool) Put(host string, c net.Conn, shouldClose bool) {
	if shouldClose {
		c.Close()
		return
	}
	ch := p.getPool(host)
	pc := &pooledConn{Conn: c, lastUsed: time.Now()}
	select {
	case ch <- pc:
		// 塞进去了
	default:
		// 池满 → 关
		c.Close()
	}
}

func (p *ConnPool) getPool(host string) chan *pooledConn {
	p.mu.Lock()
	defer p.mu.Unlock()
	ch, ok := p.pools[host]
	if !ok {
		ch = make(chan *pooledConn, p.maxPer)
		p.pools[host] = ch
	}
	return ch
}

// Close 关掉池里所有连接
func (p *ConnPool) Close() {
	p.mu.Lock()
	defer p.mu.Unlock()
	for _, ch := range p.pools {
		close(ch)
		for pc := range ch {
			pc.Close()
		}
	}
	p.pools = make(map[string]chan *pooledConn)
}

// Stats 池状态 (调试用)
func (p *ConnPool) Stats() map[string]int {
	p.mu.Lock()
	defer p.mu.Unlock()
	m := make(map[string]int, len(p.pools))
	for h, ch := range p.pools {
		m[h] = len(ch)
	}
	return m
}
