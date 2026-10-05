// pkg/mux/epoll_linux.go - Linux epoll 实现 (LT + ET 两种模式)
//
// 第 4 次会话 Step 8.2-8.3
//
// 核心流程:
//   1. epoll_create1() 建一个 epoll 实例
//   2. epoll_ctl(ADD) 把 listener fd 注册进去,等 EPOLLIN
//   3. epoll_wait() 阻塞等事件
//   4. 事件返回: 若是 listener fd → accept + 把新 fd 注册进 epoll
//               否则 → handler 处理读
//
// LT (Level-Triggered,水平触发): 只要 fd 可读,每次 epoll_wait 都通知
// ET (Edge-Triggered,边沿触发):  fd 从 "不可读" 变 "可读" 的那一瞬间才通知
//                                 必须 read 到 EAGAIN 才知道这次通知的所有数据
//
// 🔥 BUG-4 的根源:
//   ET 模式下只 read 一次 → 剩余数据不会再通知 → 看起来 "消息吞一半"
//
//go:build linux

package mux

import (
	"errors"
	"fmt"
	"net"
	"sync"
	"sync/atomic"
	"syscall"
)

// EpollReactor 单线程 epoll 事件循环
// 简化: 不做 CPU 亲和、不做多 reactor 线程池
type EpollReactor struct {
	epfd  int
	lnFd  int
	useET bool // true=ET false=LT

	totalConn  atomic.Int64
	activeConn atomic.Int64
	shutdown   atomic.Bool

	// 为了演示 BUG-4: 控制 ET 模式下 "每次只 read 多少字节"
	// 0 = 正常 (loop read 到 EAGAIN) ; >0 = 每次最多这么多字节 (制造 BUG)
	etReadLimit int

	connsMu sync.Mutex
	conns   map[int]net.Conn
}

// NewEpollReactor
// mode: "LT" | "ET" | "ET-bug"  ← ET-bug: 每次只 read 一次 (BUG-4 现场)
func NewEpollReactor(mode string) *EpollReactor {
	r := &EpollReactor{conns: make(map[int]net.Conn)}
	switch mode {
	case "ET":
		r.useET = true
	case "ET-bug":
		r.useET = true
		r.etReadLimit = 64 // 每次只 read 64 字节,凸显 BUG
	}
	return r
}

func (r *EpollReactor) Serve(ln net.Listener, h Handler) error {
	tcpLn, ok := ln.(*net.TCPListener)
	if !ok {
		return errors.New("epoll reactor requires *net.TCPListener")
	}
	f, err := tcpLn.File()
	if err != nil {
		return err
	}
	defer f.Close()
	r.lnFd = int(f.Fd())

	// 把 listener fd 设为 nonblocking (ET 必须;LT 也建议)
	if err := syscall.SetNonblock(r.lnFd, true); err != nil {
		return fmt.Errorf("set listener nonblock: %w", err)
	}

	r.epfd, err = syscall.EpollCreate1(syscall.EPOLL_CLOEXEC)
	if err != nil {
		return fmt.Errorf("epoll_create1: %w", err)
	}
	defer syscall.Close(r.epfd)

	// listener 用 LT 更简单 (ET 下 Accept 要 loop 到 EAGAIN)
	ev := syscall.EpollEvent{
		Events: syscall.EPOLLIN,
		Fd:     int32(r.lnFd),
	}
	if err := syscall.EpollCtl(r.epfd, syscall.EPOLL_CTL_ADD, r.lnFd, &ev); err != nil {
		return fmt.Errorf("epoll_ctl add listener: %w", err)
	}

	events := make([]syscall.EpollEvent, 128)
	for {
		if r.shutdown.Load() {
			return nil
		}
		// 100ms 超时,让 Shutdown 能定期检查到
		n, err := syscall.EpollWait(r.epfd, events, 100)
		if err != nil {
			if errors.Is(err, syscall.EINTR) {
				continue
			}
			return fmt.Errorf("epoll_wait: %w", err)
		}
		for i := 0; i < n; i++ {
			fd := int(events[i].Fd)
			if fd == r.lnFd {
				r.doAccept(h)
			} else {
				r.doRead(fd, h)
			}
		}
	}
}

func (r *EpollReactor) doAccept(h Handler) {
	for {
		// 真实 Linux 的 accept4 可以直接设 nonblock + CLOEXEC
		// Go 的 syscall 层用 accept
		nfd, _, err := syscall.Accept(r.lnFd)
		if err != nil {
			if errors.Is(err, syscall.EAGAIN) || errors.Is(err, syscall.EWOULDBLOCK) {
				return
			}
			return
		}
		if err := syscall.SetNonblock(nfd, true); err != nil {
			syscall.Close(nfd)
			continue
		}
		// 构造一个 net.Conn 包装 fd (用 File 转回去)
		conn, err := fileConnFromFd(nfd)
		if err != nil {
			syscall.Close(nfd)
			continue
		}

		var evFlags uint32 = syscall.EPOLLIN
		if r.useET {
			evFlags |= 1 << 31 // EPOLLET = 1 << 31
		}
		ev := syscall.EpollEvent{Events: evFlags, Fd: int32(nfd)}
		if err := syscall.EpollCtl(r.epfd, syscall.EPOLL_CTL_ADD, nfd, &ev); err != nil {
			conn.Close()
			continue
		}

		r.connsMu.Lock()
		r.conns[nfd] = conn
		r.connsMu.Unlock()
		r.totalConn.Add(1)
		r.activeConn.Add(1)
		_ = h // handler 在 doRead 里调
	}
}

func (r *EpollReactor) doRead(fd int, h Handler) {
	r.connsMu.Lock()
	conn, ok := r.conns[fd]
	r.connsMu.Unlock()
	if !ok {
		return
	}

	// LT 模式: 调用一次 handler,下次 epoll_wait 还会提醒
	// ET 正常: 循环调用 handler 直到返回 EAGAIN/EOF
	// ET-bug : 只调一次 (模拟 BUG-4)
	if !r.useET || r.etReadLimit > 0 {
		if err := h(conn); err != nil {
			r.removeConn(fd, conn)
		}
		return
	}
	// ET 正常: 必须 drain 到 EAGAIN
	for {
		err := h(conn)
		if err == nil {
			continue
		}
		if errors.Is(err, syscall.EAGAIN) || errors.Is(err, syscall.EWOULDBLOCK) {
			return // 吸完了
		}
		r.removeConn(fd, conn)
		return
	}
}

func (r *EpollReactor) removeConn(fd int, conn net.Conn) {
	syscall.EpollCtl(r.epfd, syscall.EPOLL_CTL_DEL, fd, nil)
	conn.Close()
	r.connsMu.Lock()
	delete(r.conns, fd)
	r.connsMu.Unlock()
	r.activeConn.Add(-1)
}

func (r *EpollReactor) Shutdown() error {
	r.shutdown.Store(true)
	return nil
}

func (r *EpollReactor) Stats() (int64, int64) {
	return r.totalConn.Load(), r.activeConn.Load()
}

// ETReadLimit 测试用: 让 handler 知道每次最多读多少字节
// (handler 配合: 如果 read 结果 >= limit 就假装读完,等下次 epoll 通知 → 制造 BUG-4)
func (r *EpollReactor) ETReadLimit() int { return r.etReadLimit }

// fileConnFromFd 把 fd 包成 net.Conn
func fileConnFromFd(fd int) (net.Conn, error) {
	f := osNewFile(uintptr(fd), "epoll-conn")
	defer f.Close()
	return net.FileConn(f)
}
