package mux_test

import (
	"bytes"
	"io"
	"net"
	"runtime"
	"strings"
	"sync"
	"testing"
	"time"

	"mininet/pkg/mux"
)

// echoHandler 一次调用读一次,返回错误(含 EOF)就让 reactor 关连接
func echoHandler(c net.Conn) error {
	buf := make([]byte, 4096)
	n, err := c.Read(buf)
	if n > 0 {
		if _, werr := c.Write(buf[:n]); werr != nil {
			return werr
		}
	}
	return err
}

func TestGoroutineReactorEcho(t *testing.T) {
	ln, err := net.Listen("tcp", "127.0.0.1:0")
	if err != nil {
		t.Fatal(err)
	}
	addr := ln.Addr().String()
	r := mux.NewGoroutineReactor()

	go func() { r.Serve(ln, echoHandler) }()
	defer func() {
		r.Shutdown()
		ln.Close()
	}()
	time.Sleep(20 * time.Millisecond)

	// 10 并发,每个发一条短消息,期望 echo 回来
	var wg sync.WaitGroup
	for i := 0; i < 10; i++ {
		wg.Add(1)
		go func(i int) {
			defer wg.Done()
			c, err := net.Dial("tcp", addr)
			if err != nil {
				t.Errorf("dial: %v", err)
				return
			}
			defer c.Close()
			msg := []byte(strings.Repeat("x", 20))
			c.Write(msg)
			buf := make([]byte, 100)
			c.SetReadDeadline(time.Now().Add(1 * time.Second))
			n, _ := c.Read(buf)
			if !bytes.Equal(buf[:n], msg) {
				t.Errorf("goroutine %d echo mismatch", i)
			}
		}(i)
	}
	wg.Wait()

	total, _ := r.Stats()
	if total < 10 {
		t.Errorf("totalConn = %d, want >= 10", total)
	}
	t.Logf("goroutine reactor served %d conns", total)
}

// epoll 相关测试只在 Linux 跑
func TestEpollReactorEcho(t *testing.T) {
	if runtime.GOOS != "linux" {
		t.Skipf("epoll reactor requires linux, got %s", runtime.GOOS)
	}
	ln, err := net.Listen("tcp", "127.0.0.1:0")
	if err != nil {
		t.Fatal(err)
	}
	addr := ln.Addr().String()
	r := mux.NewEpollReactor("LT")

	go func() { r.Serve(ln, echoHandler) }()
	defer func() {
		r.Shutdown()
		ln.Close()
		time.Sleep(150 * time.Millisecond)
	}()
	time.Sleep(50 * time.Millisecond)

	c, err := net.Dial("tcp", addr)
	if err != nil {
		t.Fatal(err)
	}
	defer c.Close()
	msg := []byte("hello epoll!")
	c.Write(msg)
	buf := make([]byte, 100)
	c.SetReadDeadline(time.Now().Add(1 * time.Second))
	n, _ := c.Read(buf)
	if !bytes.Equal(buf[:n], msg) {
		t.Errorf("echo mismatch: got %q", buf[:n])
	}
	t.Logf("epoll LT reactor echo ok")

	// 简单读到 EOF 关
	c.Close()
	_ = io.EOF
}
