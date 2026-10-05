// pkg/link/loopback.go - 用两个 chan 交叉连接的 Loopback 驱动
//
// 第 2 次会话 Step 4.1
//
// 设计:两个 Loopback 实例共享两条 chan,各自的 tx 对应对端的 rx:
//
//     +--------+   aToB chan   +--------+
//     |        | ---tx -----> |        |
//     | side A |              | side B |
//     |        | <-- rx ----- |        |
//     +--------+   bToA chan   +--------+
//
// 优点: 零 syscall · 跨平台 · CI 友好 · goroutine-safe
// 缺点: 不是"真"网络,但 L2 以上代码完全一致
package link

import (
	"sync"
	"sync/atomic"
)

// Loopback 用 chan 实现的 Driver
// 每个实例有独立的 closed 状态和 close 信号,互不干扰
type Loopback struct {
	mac       MAC
	tx        chan<- []byte // 发出去 (对端的 rx)
	rx        <-chan []byte // 收进来 (对端的 tx)
	closed    atomic.Bool
	closeOnce sync.Once
	closeChan chan struct{} // 本实例独有,Close 时 close 它唤醒本地 Send/Recv
}

// LoopbackPair 创建一对已连接的 Loopback 驱动
// 返回 (a, b),a.Send 发出的字节会从 b.Recv 读到,反之亦然
//
// 两个实例各自独立持有 close 信号 → 任何一方 Close 不影响对端;
// 对端继续 Send 到无人接收的 chan 时会被 chan 的缓冲吸收,
// 读完缓冲后 Recv 会阻塞 (符合 "对端消失但本地还活着" 的现实)。
//
// 用法:
//
//	a, b := LoopbackPair(ParseMAC("aa:bb:cc:dd:ee:01"),
//	                     ParseMAC("aa:bb:cc:dd:ee:02"))
//	defer a.Close(); defer b.Close()
//	a.Send([]byte{...})
//	frame, _ := b.Recv()
func LoopbackPair(macA, macB MAC) (*Loopback, *Loopback) {
	// 缓冲 32:避免发送方被偶尔的消费延迟堵住
	// (真实网卡有 ring buffer,这里对应其缓冲)
	aToB := make(chan []byte, 32)
	bToA := make(chan []byte, 32)

	return &Loopback{
			mac: macA, tx: aToB, rx: bToA,
			closeChan: make(chan struct{}),
		}, &Loopback{
			mac: macB, tx: bToA, rx: aToB,
			closeChan: make(chan struct{}),
		}
}

func (l *Loopback) MAC() MAC { return l.mac }

func (l *Loopback) Send(buf []byte) error {
	if l.closed.Load() {
		return ErrDriverClosed
	}
	// 复制一份:避免调用方后续修改原 buf 影响对端
	// (真实网卡 DMA 也是把 skb 拷进硬件缓冲)
	cp := make([]byte, len(buf))
	copy(cp, buf)
	select {
	case l.tx <- cp:
		return nil
	case <-l.closeChan:
		return ErrDriverClosed
	}
}

func (l *Loopback) Recv() ([]byte, error) {
	if l.closed.Load() {
		return nil, ErrDriverClosed
	}
	select {
	case buf, ok := <-l.rx:
		if !ok {
			return nil, ErrDriverClosed
		}
		return buf, nil
	case <-l.closeChan:
		return nil, ErrDriverClosed
	}
}

func (l *Loopback) Close() error {
	l.closeOnce.Do(func() {
		l.closed.Store(true)
		close(l.closeChan)
	})
	return nil
}
