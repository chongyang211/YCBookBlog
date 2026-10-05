// cmd/mnet/tcp.go - REPL 的 tcp 命令 (阶段 ⑥⑦)
package main

import (
	"fmt"
	"io"
	"strconv"
	"time"

	netpkg "mininet/pkg/net"
	"mininet/pkg/transport"
)

// handleTCP: tcp listen <port> | tcp connect <ip>:<port> | tcp show
func handleTCP(s *Stack, args []string) {
	if s.l4 == nil {
		fmt.Println("(error) peer not up; run `peer up` first")
		return
	}
	if len(args) == 0 {
		fmt.Println("usage:")
		fmt.Println("  tcp listen <port>              监听端口 (本机)")
		fmt.Println("  tcp connect <ip>:<port>        连接 (本机 → peer)")
		fmt.Println("  tcp show                       打印所有 TCB 状态")
		return
	}
	switch args[0] {
	case "listen":
		if len(args) < 2 {
			fmt.Println("usage: tcp listen <port>")
			return
		}
		port, err := strconv.Atoi(args[1])
		if err != nil || port <= 0 || port > 65535 {
			fmt.Println("(error) bad port")
			return
		}
		ln, err := s.l4.Listen(uint16(port))
		if err != nil {
			fmt.Println("(error)", err)
			return
		}
		fmt.Printf("listening on :%d (background echo server)\n", port)
		// 后台起一个 echo server
		go runEchoServer(ln)

	case "connect":
		if len(args) < 2 {
			fmt.Println("usage: tcp connect <ip>:<port>")
			return
		}
		ipStr, portStr := splitHostPort(args[1])
		ip, err := netpkg.ParseIPv4(ipStr)
		if err != nil {
			fmt.Println("(error) ip:", err)
			return
		}
		port, err := strconv.Atoi(portStr)
		if err != nil {
			fmt.Println("(error) port:", err)
			return
		}
		fmt.Printf("connecting to %s:%d ...\n", ip, port)
		t0 := time.Now()
		c, err := s.l4.Dial(ip, uint16(port), 3*time.Second)
		if err != nil {
			fmt.Println("(error)", err)
			return
		}
		fmt.Printf("connected (state=%s, took %v, local port=%d)\n",
			c.State(), time.Since(t0), c.LocalPort())
		// 发一个测试字符串,读 echo 回来 (如果对面是 echo server)
		msg := []byte("hello from mnet REPL!")
		if _, err := c.Write(msg); err == nil {
			buf := make([]byte, 128)
			c.Read(buf)
		}
		c.Close()
		fmt.Println("closed.")

	case "show":
		fmt.Println(s.l4.Dump())

	default:
		fmt.Println("unknown tcp subcommand:", args[0])
	}
}

// splitHostPort "1.2.3.4:80" → ("1.2.3.4", "80")
func splitHostPort(s string) (string, string) {
	for i := len(s) - 1; i >= 0; i-- {
		if s[i] == ':' {
			return s[:i], s[i+1:]
		}
	}
	return s, ""
}

// runEchoServer 后台 echo server (每个连接独立 goroutine)
func runEchoServer(ln *transport.Listener) {
	for {
		c, err := ln.Accept()
		if err != nil {
			return
		}
		go func(c *transport.Conn) {
			defer c.Close()
			buf := make([]byte, 4096)
			for {
				n, err := c.Read(buf)
				if n > 0 {
					c.Write(buf[:n])
				}
				if err == io.EOF || err != nil {
					return
				}
			}
		}(c)
	}
}
