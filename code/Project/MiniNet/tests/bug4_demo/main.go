// tests/bug4_demo/main.go - 🔥 BUG-4 现场:epoll ET 模式只 read 一次
//
// 跑法:
//   # 必须 Linux (或用 Docker/VM/WSL)
//   go run ./tests/bug4_demo
//   # 或
//   make bug4-demo
//
// macOS 用户: 跳过(demo 会提示并退出 0)
//
// 本 demo 展示的坑:
//   epoll ET (边沿触发) 只在 "fd 从不可读变可读" 的瞬间通知一次。
//   如果 handler 只 read 一次就返回,剩余数据永远不会再被通知,
//   表现为 "HTTP 响应随机截断"、"收到一半消息"。
//
// 对比:
//   ❌ BUG 版: ET 模式,handler 每次只读 64 字节,余下丢失
//   ✅ 修复版: ET 模式,handler 循环 read 到 EAGAIN
package main

import (
	"errors"
	"fmt"
	"io"
	"net"
	"runtime"
	"sync/atomic"
	"syscall"
	"time"

	"mininet/pkg/common"
	"mininet/pkg/mux"
)

func main() {
	if runtime.GOOS != "linux" {
		fmt.Println("⚠️  BUG-4 demo 只在 Linux 上有意义 (epoll)")
		fmt.Println("    macOS 用户可用 Docker/VM/WSL 跑")
		fmt.Printf("    当前系统: %s\n", runtime.GOOS)
		return
	}
	common.SetLogLevel(common.LvInfo)

	fmt.Println("========== 🔥 BUG-4 版 (ET + 每次只 read 64B) ==========")
	runOne("ET-bug")
	fmt.Println()
	fmt.Println("========== ✅ 修复版 (ET + loop read 到 EAGAIN) ==========")
	runOne("ET")
	fmt.Println()
	fmt.Println("========== 📊 结论 ==========")
	fmt.Println("ET (边沿触发) 的唯一正确用法: handler 必须 loop read 到 EAGAIN。")
	fmt.Println("Nginx/Netty/Go runtime 的 netpoller 都严格遵守这一点。")
	fmt.Println("只 read 一次 → 剩余数据永远收不到 → 表现为 '随机截断'")
}

func runOne(mode string) {
	ln, err := net.Listen("tcp", "127.0.0.1:0")
	if err != nil {
		fmt.Println("listen:", err)
		return
	}
	addr := ln.Addr().String()
	r := mux.NewEpollReactor(mode)

	var totalRecv atomic.Int64
	handler := func(c net.Conn) error {
		// 不管什么模式,都用小 buf 读一次
		buf := make([]byte, 64)
		n, err := c.Read(buf)
		if n > 0 {
			totalRecv.Add(int64(n))
		}
		if err != nil {
			return err // EOF / EAGAIN 会让 reactor 决定关或继续
		}
		return nil
	}

	go func() {
		if err := r.Serve(ln, handler); err != nil {
			fmt.Println("serve error:", err)
		}
	}()
	defer func() {
		r.Shutdown()
		ln.Close()
		time.Sleep(200 * time.Millisecond)
	}()
	time.Sleep(50 * time.Millisecond)

	// 客户端:一次发 2 KB,看服务端收到多少
	c, err := net.Dial("tcp", addr)
	if err != nil {
		fmt.Println("dial:", err)
		return
	}
	payload := make([]byte, 2048)
	for i := range payload {
		payload[i] = byte(i % 256)
	}
	t0 := time.Now()
	if _, err := c.Write(payload); err != nil {
		fmt.Println("write:", err)
		c.Close()
		return
	}
	// 等一会让服务端消化
	time.Sleep(500 * time.Millisecond)
	c.Close()
	time.Sleep(300 * time.Millisecond)

	got := totalRecv.Load()
	fmt.Printf("客户端发送 2048 字节,服务端收到 %d 字节 (%v)\n",
		got, time.Since(t0))
	if got >= 2048 {
		fmt.Println("✅ 全部收到")
	} else {
		fmt.Printf("💥 丢了 %d 字节 ← BUG!\n", 2048-got)
	}
	if errors.Is(err, io.EOF) || errors.Is(err, syscall.EINVAL) {
		// avoid unused imports warnings in some paths
	}
}
