// pkg/ws/conn.go - WebSocket 连接封装 (收发帧 + ping 心跳)
//
// 第 6 次会话 Step 14.3
//
// 对外 API 类似 gorilla/websocket:
//   c.WriteText("hello")
//   c.ReadMessage() → (opcode, payload, err)
//   c.StartHeartbeat(interval) → 自动定时发 ping, 并处理回来的 pong
//   c.Close()
package ws

import (
	"bufio"
	"fmt"
	"net"
	"sync"
	"time"

	"mininet/pkg/common"
)

// Conn WebSocket 连接
type Conn struct {
	conn     net.Conn
	br       *bufio.Reader
	isServer bool // server 发帧不 mask, client 发帧要 mask

	writeMu sync.Mutex // 并发安全写

	// 心跳
	hbStop     chan struct{}
	hbInterval time.Duration
	lastPong   time.Time
	pingSent   int
	pongRecv   int
}

// Read 读一个消息 (自动处理控制帧)
// 遇到 ping 自动回 pong, 遇到 close 返回 io.EOF
func (c *Conn) Read() (Opcode, []byte, error) {
	for {
		f, err := ReadFrame(c.br)
		if err != nil {
			return 0, nil, err
		}
		switch f.Opcode {
		case OpPing:
			// 自动回 pong (用 ping 的 payload)
			c.writePong(f.Payload)
			continue
		case OpPong:
			c.pongRecv++
			c.lastPong = time.Now()
			continue
		case OpClose:
			// 回 close 并返回 EOF
			c.sendClose(1000, "normal")
			return OpClose, f.Payload, fmt.Errorf("ws closed by peer")
		case OpText, OpBinary:
			return f.Opcode, f.Payload, nil
		case OpContinuation:
			// 教学简化: 不实现分片组装, 直接报错
			return 0, nil, fmt.Errorf("continuation frames not supported")
		}
	}
}

// WriteText 发文本
func (c *Conn) WriteText(s string) error {
	return c.writeFrame(&Frame{Fin: true, Opcode: OpText, Payload: []byte(s)})
}

// WriteBinary 发二进制
func (c *Conn) WriteBinary(b []byte) error {
	return c.writeFrame(&Frame{Fin: true, Opcode: OpBinary, Payload: b})
}

// Ping 主动 ping (StartHeartbeat 会自动调)
func (c *Conn) Ping(payload []byte) error {
	c.pingSent++
	return c.writeFrame(&Frame{Fin: true, Opcode: OpPing, Payload: payload})
}

func (c *Conn) writePong(payload []byte) error {
	return c.writeFrame(&Frame{Fin: true, Opcode: OpPong, Payload: payload})
}

func (c *Conn) sendClose(code uint16, reason string) {
	payload := make([]byte, 2+len(reason))
	payload[0] = byte(code >> 8)
	payload[1] = byte(code)
	copy(payload[2:], reason)
	c.writeFrame(&Frame{Fin: true, Opcode: OpClose, Payload: payload})
}

func (c *Conn) writeFrame(f *Frame) error {
	c.writeMu.Lock()
	defer c.writeMu.Unlock()
	return WriteFrame(c.conn, f, !c.isServer)
}

// Close 优雅关闭
func (c *Conn) Close() error {
	c.StopHeartbeat()
	c.sendClose(1000, "bye")
	return c.conn.Close()
}

// StartHeartbeat 启动心跳 (每 interval 发一次 ping)
// Nginx/云 LB 默认 60s 空闲会 RST,所以推荐 interval=30s
func (c *Conn) StartHeartbeat(interval time.Duration) {
	c.hbInterval = interval
	c.hbStop = make(chan struct{})
	c.lastPong = time.Now()
	go func() {
		tk := time.NewTicker(interval)
		defer tk.Stop()
		for {
			select {
			case <-c.hbStop:
				return
			case now := <-tk.C:
				if err := c.Ping([]byte("heartbeat")); err != nil {
					common.Trace("ws", "ping failed: %v", err)
					return
				}
				// 超过 3 个心跳周期没收到 pong → 认为死
				if now.Sub(c.lastPong) > 3*interval {
					common.Trace("ws", "pong timeout, closing")
					c.conn.Close()
					return
				}
			}
		}
	}()
}

// StopHeartbeat
func (c *Conn) StopHeartbeat() {
	if c.hbStop != nil {
		close(c.hbStop)
		c.hbStop = nil
	}
}

// HeartbeatStats
func (c *Conn) HeartbeatStats() (pingSent, pongRecv int, lastPong time.Time) {
	return c.pingSent, c.pongRecv, c.lastPong
}

// RawConn 底层 net.Conn (给 Reactor/诊断用)
func (c *Conn) RawConn() net.Conn { return c.conn }
