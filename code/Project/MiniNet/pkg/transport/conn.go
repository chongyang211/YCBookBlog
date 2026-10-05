// pkg/transport/conn.go - Conn 对外 API (Read/Write/Close)
//
// 第 3 次会话 Step 6.3-6.4
//
// Conn 和 Go 标准库 net.Conn 的接口基本一致。
//
// 简化:
//   - Read: 从 recvBuf 拷,没有就阻塞等 readable chan
//   - Write: 直接发 (按 MSS 分片),没有发送窗口节流 (§⑦ 加)
//   - 没有 Deadline 支持 (阶段 ⑫ 加)
package transport

import (
	"fmt"
	"io"
	"time"
)

// Conn 一条 TCP 连接
type Conn struct {
	tcb *TCB
	l4  *L4Layer
}

// LocalAddr / RemoteAddr
func (c *Conn) LocalPort() uint16  { return c.tcb.Tuple.LocalPort }
func (c *Conn) RemotePort() uint16 { return c.tcb.Tuple.RemotePort }

// State 当前状态 (给诊断用)
func (c *Conn) State() TCPState {
	c.tcb.mu.Lock()
	defer c.tcb.mu.Unlock()
	return c.tcb.State
}

// Read 读数据,语义同 io.Reader
// EOF: 对端 FIN 且本地缓冲耗尽时返回 io.EOF
func (c *Conn) Read(p []byte) (int, error) {
	for {
		c.tcb.mu.Lock()
		if len(c.tcb.recvBuf) > 0 {
			n := copy(p, c.tcb.recvBuf)
			c.tcb.recvBuf = c.tcb.recvBuf[n:]
			c.tcb.mu.Unlock()
			return n, nil
		}
		if c.tcb.peerClosed {
			c.tcb.mu.Unlock()
			return 0, io.EOF
		}
		if c.tcb.State == StateClosed {
			c.tcb.mu.Unlock()
			return 0, ErrClosed
		}
		c.tcb.mu.Unlock()
		// 阻塞等
		<-c.tcb.readable
	}
}

// Write 写数据,按 MSS 分片发出
// 返回实际写入的字节数,或错误
func (c *Conn) Write(p []byte) (int, error) {
	total := 0
	for len(p) > 0 {
		c.tcb.mu.Lock()
		if c.tcb.State != StateEstablished && c.tcb.State != StateCloseWait {
			c.tcb.mu.Unlock()
			return total, ErrNotEstablished
		}
		chunk := p
		if len(chunk) > MSS {
			chunk = chunk[:MSS]
		}
		err := c.l4.sendSegment(c.tcb, FlagACK|FlagPSH, chunk)
		if err != nil {
			c.tcb.mu.Unlock()
			return total, err
		}
		c.tcb.mu.Unlock()
		total += len(chunk)
		p = p[len(chunk):]
	}
	return total, nil
}

// Close 主动关闭 (发 FIN)
// 根据当前状态走不同分支
//
// 教学注意:真实 TCP 的 FIN seq 在数据之后,接收端按序处理;
// 如果前面有未 ACK 的数据,FIN 到了会被接收端缓存等待重传填补。
// 我们的接收端不做乱序缓存(简化),所以 Close 要先等 rtq 清空再发 FIN,
// 否则 FIN 到了但数据没到 → 接收端会错过数据。
func (c *Conn) Close() error {
	// 等所有已发数据被 ACK (给 RTO 重传留时间)
	c.waitRtqDrain(5 * 1000) // 5s 上限

	c.tcb.mu.Lock()
	defer c.tcb.mu.Unlock()
	switch c.tcb.State {
	case StateEstablished:
		c.tcb.setState(StateFinWait1)
		c.l4.sendSegment(c.tcb, FlagFIN|FlagACK, nil)
	case StateCloseWait:
		c.tcb.setState(StateLastAck)
		c.l4.sendSegment(c.tcb, FlagFIN|FlagACK, nil)
	case StateClosed:
		return nil
	default:
		return fmt.Errorf("close in state %s not supported", c.tcb.State)
	}
	return nil
}

// waitRtqDrain 轮询等重传队列清空,最多 maxMs 毫秒
func (c *Conn) waitRtqDrain(maxMs int) {
	for i := 0; i < maxMs/20; i++ {
		c.tcb.mu.Lock()
		if c.tcb.rtq == nil || c.tcb.rtq.Len() == 0 {
			c.tcb.mu.Unlock()
			return
		}
		c.tcb.mu.Unlock()
		time.Sleep(20 * time.Millisecond)
	}
}

// WaitClosed 阻塞到完全关闭 (给测试用)
func (c *Conn) WaitClosed() {
	<-c.tcb.closedCh
}
