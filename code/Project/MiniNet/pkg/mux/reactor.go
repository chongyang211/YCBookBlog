// pkg/mux/reactor.go - Reactor 接口 + 三种实现 (goroutine/epoll_LT/epoll_ET)
//
// 第 4 次会话 Step 8.1-8.3
//
// 为什么三种实现一起放?
//   - 教学目的: 对比"每连接一协程"vs"单线程 epoll"的 CPU/内存开销
//   - 工业事实: Go runtime 底层就是 epoll (netpoller),goroutine 模型只是封装
//   - 本案例让你亲手写一次 epoll,理解 Go/Nginx/Node.js 底层做了什么
//
// 本模块 **不** 走我们自己的 L4 TCP 栈,直接用 OS socket (net.Listener).
// 原因:教学 epoll 的核心是"用户态拿一堆 fd 问 kernel 谁 ready",
//      只有真实 fd 才有这个 API;我们的 L4 Conn 是 goroutine + chan,
//      天然就是 "goroutine-per-conn" 模型,无法示范 ET/LT 区别。
package mux

import (
	"errors"
	"io"
	"net"
	"sync/atomic"
)

// Handler 一次 "有数据可读" 的回调
// 返回错误 → reactor 关闭该连接
type Handler func(conn net.Conn) error

// Reactor 事件循环接口
type Reactor interface {
	// Serve 阻塞运行,直到 Shutdown 或致命错误
	Serve(ln net.Listener, h Handler) error
	// Shutdown 优雅停止 (让 Serve 返回 nil)
	Shutdown() error
	// Stats 返回已处理的连接数 / 当前活跃数
	Stats() (totalConn, activeConn int64)
}

// -------- 实现 1: goroutine-per-connection --------
//
// 这是 Go 原生写法: Accept 一个新连接 → go handleConn(conn)
// 每条连接独享一个 goroutine,Read 阻塞时 Go runtime 自动 park 它。
// 简单直接,但高并发 (10k+) 时 goroutine 调度开销显著。

type GoroutineReactor struct {
	totalConn  atomic.Int64
	activeConn atomic.Int64
	shutdown   atomic.Bool
	done       chan struct{}
}

func NewGoroutineReactor() *GoroutineReactor {
	return &GoroutineReactor{done: make(chan struct{})}
}

func (r *GoroutineReactor) Serve(ln net.Listener, h Handler) error {
	defer close(r.done)
	for {
		conn, err := ln.Accept()
		if err != nil {
			if r.shutdown.Load() {
				return nil
			}
			if errors.Is(err, net.ErrClosed) {
				return nil
			}
			return err
		}
		r.totalConn.Add(1)
		r.activeConn.Add(1)
		go func(c net.Conn) {
			defer func() {
				c.Close()
				r.activeConn.Add(-1)
			}()
			for {
				if err := h(c); err != nil {
					if err != io.EOF {
						// trace 级别的 err 不打印
					}
					return
				}
			}
		}(conn)
	}
}

func (r *GoroutineReactor) Shutdown() error {
	r.shutdown.Store(true)
	return nil
}

func (r *GoroutineReactor) Stats() (int64, int64) {
	return r.totalConn.Load(), r.activeConn.Load()
}
