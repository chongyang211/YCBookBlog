// tests/bug6_demo/main.go - BUG-6 WebSocket 没心跳 → LB 空闲 RST
//
// 跑法: make bug6-demo
//
// 真实场景:
//   前端通过 Nginx/云 LB 连后端 WebSocket
//   如果 60s 内无流量,LB 认为连接死了 → 发 RST
//   前端收到 RST → 聊天断线
//
// 我们本地复现:
//   做一个"假 LB":它在中间抓包 TCP,看到 2s 内 WS 无帧 → 自动断
//   客户端 A: 不开心跳 → 2s 后被 LB 杀
//   客户端 B: 开 1s 心跳 → 永远不被杀
package main

import (
	"bufio"
	"fmt"
	"io"
	"net"
	"time"

	"mininet/pkg/common"
	mhttp "mininet/pkg/http"
	"mininet/pkg/ws"
)

const idleTimeout = 2 * time.Second

// 假 LB: 转发两边, 空闲 > idleTimeout 就断
func startFakeLB(backend string) string {
	ln, _ := net.Listen("tcp", "127.0.0.1:0")
	go func() {
		for {
			c, err := ln.Accept()
			if err != nil {
				return
			}
			go handleLB(c, backend)
		}
	}()
	return ln.Addr().String()
}

func handleLB(client net.Conn, backend string) {
	defer client.Close()
	up, err := net.Dial("tcp", backend)
	if err != nil {
		return
	}
	defer up.Close()

	// 两侧 pump,带空闲检测
	lastActivity := time.Now()
	done := make(chan struct{}, 2)

	pump := func(dst, src net.Conn) {
		buf := make([]byte, 4096)
		for {
			src.SetReadDeadline(time.Now().Add(500 * time.Millisecond))
			n, err := src.Read(buf)
			if n > 0 {
				lastActivity = time.Now()
				dst.Write(buf[:n])
			}
			if err != nil {
				if netErr, ok := err.(net.Error); ok && netErr.Timeout() {
					if time.Since(lastActivity) > idleTimeout {
						fmt.Printf("  💥 LB: 连接 %s 空闲 >%v, 发 RST\n",
							src.RemoteAddr(), idleTimeout)
						// 模拟 RST: tcp set linger=0 后关
						if tcp, ok := src.(*net.TCPConn); ok {
							tcp.SetLinger(0)
						}
						src.Close()
						done <- struct{}{}
						return
					}
					continue
				}
				done <- struct{}{}
				return
			}
		}
	}
	go pump(up, client)
	go pump(client, up)
	<-done
}

func main() {
	common.SetLogLevel(common.LvWarn)
	fmt.Println("================================================================")
	fmt.Println("🔥 BUG-6: WebSocket 无心跳 → LB 空闲 2s 后 RST")
	fmt.Println("================================================================")

	// 启动后端 WS server
	backLn, _ := net.Listen("tcp", "127.0.0.1:0")
	go func() {
		for {
			c, err := backLn.Accept()
			if err != nil {
				return
			}
			go func(conn net.Conn) {
				br := bufio.NewReader(conn)
				req, err := mhttp.ReadRequest(br)
				if err != nil {
					return
				}
				wsc, err := ws.Upgrade(conn, br, req)
				if err != nil {
					return
				}
				for {
					op, payload, err := wsc.Read()
					if err != nil {
						return
					}
					if op == ws.OpText {
						wsc.WriteText("echo:" + string(payload))
					}
				}
			}(c)
		}
	}()
	time.Sleep(30 * time.Millisecond)

	// 启动假 LB
	lbAddr := startFakeLB(backLn.Addr().String())
	time.Sleep(30 * time.Millisecond)
	fmt.Printf("假 LB 启动在 %s, 后端 %s, 空闲阈值 %v\n\n", lbAddr, backLn.Addr(), idleTimeout)

	// ---------- 场景 A: 不开心跳 ----------
	fmt.Println("----- 🔥 场景 A: 不开心跳 -----")
	runA := func() {
		c, err := ws.Dial("ws://" + lbAddr + "/chat")
		if err != nil {
			fmt.Printf("  连接失败: %v\n", err)
			return
		}
		c.WriteText("hi")
		op, msg, _ := c.Read()
		fmt.Printf("  t=0.0s   收到 %s: %q\n", op, msg)

		// 静默等 3 秒 (> idleTimeout)
		time.Sleep(3 * time.Second)
		fmt.Printf("  t=3.0s   尝试再发消息...\n")
		if err := c.WriteText("still alive?"); err != nil {
			fmt.Printf("  💥 WriteText 失败: %v\n", err)
			return
		}
		op, msg, err = c.Read()
		if err == io.EOF || err != nil {
			fmt.Printf("  💥 Read 失败: %v  → 连接已被 LB 杀\n", err)
			return
		}
		fmt.Printf("  收到 %s: %q (不该到这里!)\n", op, msg)
	}
	runA()

	// ---------- 场景 B: 开 1s 心跳 ----------
	fmt.Println()
	fmt.Println("----- ✅ 场景 B: 500ms 心跳 + 后台 reader 消化 pong -----")
	runB := func() {
		c, err := ws.Dial("ws://" + lbAddr + "/chat")
		if err != nil {
			fmt.Printf("  连接失败: %v\n", err)
			return
		}
		c.StartHeartbeat(500 * time.Millisecond) // < LB 窗口的 1/4,确保稳定
		defer c.Close()

		// 关键:真实应用必须有后台 reader goroutine
		// (否则 server 回的 Pong 没人消化, StartHeartbeat 的 lastPong 永远不更新 → 客户端自判死)
		msgCh := make(chan string, 10)
		errCh := make(chan error, 1)
		go func() {
			for {
				op, payload, err := c.Read()
				if err != nil {
					errCh <- err
					return
				}
				if op == ws.OpText {
					msgCh <- string(payload)
				}
			}
		}()

		c.WriteText("hi")
		select {
		case m := <-msgCh:
			fmt.Printf("  t=0.0s   收到 Text: %q\n", m)
		case e := <-errCh:
			fmt.Printf("  💥 read err: %v\n", e)
			return
		case <-time.After(1 * time.Second):
			fmt.Printf("  💥 timeout\n")
			return
		}

		// 静默等 3 秒 (但心跳一直在跑, reader 消化 pong)
		time.Sleep(3 * time.Second)
		fmt.Printf("  t=3.0s   尝试再发消息...\n")
		if err := c.WriteText("still alive?"); err != nil {
			fmt.Printf("  💥 WriteText 失败: %v\n", err)
			return
		}
		select {
		case m := <-msgCh:
			fmt.Printf("  ✅ 收到 Text: %q  → 心跳救了连接!\n", m)
		case e := <-errCh:
			fmt.Printf("  💥 read err: %v\n", e)
			return
		case <-time.After(2 * time.Second):
			fmt.Printf("  💥 timeout waiting response\n")
			return
		}
		p, pr, _ := c.HeartbeatStats()
		fmt.Printf("  心跳统计: ping 发 %d · pong 收 %d\n", p, pr)
	}
	runB()

	fmt.Println()
	fmt.Println("================================================================")
	fmt.Println("📊 结论: 公网 WS 必须开心跳 (推荐 30s < LB 默认 60s)")
	fmt.Println("        前端 JS 用 'ping' JSON 消息也行; 原生 Ping 帧更省")
	fmt.Println("================================================================")
}
