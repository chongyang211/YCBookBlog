// pkg/mux/bench_test.go - goroutine vs epoll 的 QPS 对比
//
// 跑法:
//   go test ./pkg/mux/ -bench=. -benchtime=3s
//
// macOS 用户: epoll 自动 skip
package mux_test

import (
	"net"
	"runtime"
	"testing"
	"time"

	"mininet/pkg/mux"
)

func benchReactor(b *testing.B, r mux.Reactor) {
	ln, err := net.Listen("tcp", "127.0.0.1:0")
	if err != nil {
		b.Fatal(err)
	}
	addr := ln.Addr().String()

	go func() { r.Serve(ln, echoHandler) }()
	defer func() {
		r.Shutdown()
		ln.Close()
	}()
	time.Sleep(50 * time.Millisecond)

	b.ResetTimer()
	// 单客户端串行 echo,拿 QPS 看量级 (真正并发压测自己改)
	c, err := net.Dial("tcp", addr)
	if err != nil {
		b.Fatal(err)
	}
	defer c.Close()
	msg := make([]byte, 128)
	buf := make([]byte, 256)
	for i := 0; i < b.N; i++ {
		c.Write(msg)
		c.Read(buf)
	}
	b.StopTimer()
}

func BenchmarkReactor_Goroutine(b *testing.B) {
	benchReactor(b, mux.NewGoroutineReactor())
}

func BenchmarkReactor_EpollLT(b *testing.B) {
	if runtime.GOOS != "linux" {
		b.Skip("epoll requires linux")
	}
	benchReactor(b, mux.NewEpollReactor("LT"))
}

func BenchmarkReactor_EpollET(b *testing.B) {
	if runtime.GOOS != "linux" {
		b.Skip("epoll requires linux")
	}
	benchReactor(b, mux.NewEpollReactor("ET"))
}
