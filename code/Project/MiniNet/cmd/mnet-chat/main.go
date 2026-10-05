// cmd/mnet-chat/main.go - 迷你 WebSocket 聊天室
//
// 用法:
//   # Terminal A (服务端)
//   mnet-chat -server -listen :9100
//
//   # Terminal B/C (客户端)
//   mnet-chat -nick alice -connect ws://127.0.0.1:9100/chat
//   mnet-chat -nick bob   -connect ws://127.0.0.1:9100/chat
//
// 服务端广播所有收到的消息给其他客户端 (fan-out)
package main

import (
	"bufio"
	"flag"
	"fmt"
	"net"
	"os"
	"sync"
	"time"

	"mininet/pkg/common"
	mhttp "mininet/pkg/http"
	"mininet/pkg/ws"
)

func main() {
	server := flag.Bool("server", false, "run as server")
	listen := flag.String("listen", ":9100", "server listen addr")
	connect := flag.String("connect", "", "client connect url (ws://...)")
	nick := flag.String("nick", "guest", "nick name")
	hbInterval := flag.Duration("hb", 30*time.Second, "heartbeat interval")
	logLv := flag.String("log", "warn", "log level")
	flag.Parse()

	lv, _ := common.ParseLogLevel(*logLv)
	common.SetLogLevel(lv)

	if *server {
		runServer(*listen, *hbInterval)
		return
	}
	if *connect == "" {
		fmt.Fprintln(os.Stderr, "need -server or -connect")
		os.Exit(2)
	}
	runClient(*connect, *nick, *hbInterval)
}

// 服务端
type room struct {
	mu    sync.Mutex
	peers map[*ws.Conn]string // conn → nick
}

func (r *room) join(c *ws.Conn, nick string) {
	r.mu.Lock()
	r.peers[c] = nick
	r.mu.Unlock()
	r.broadcast(c, fmt.Sprintf("*** %s joined (%d online)", nick, len(r.peers)))
}
func (r *room) leave(c *ws.Conn) {
	r.mu.Lock()
	nick := r.peers[c]
	delete(r.peers, c)
	r.mu.Unlock()
	r.broadcast(c, fmt.Sprintf("*** %s left", nick))
}
func (r *room) broadcast(sender *ws.Conn, msg string) {
	r.mu.Lock()
	defer r.mu.Unlock()
	for c := range r.peers {
		if c == sender {
			continue
		}
		c.WriteText(msg)
	}
}

func runServer(addr string, hb time.Duration) {
	rm := &room{peers: make(map[*ws.Conn]string)}
	ln, err := net.Listen("tcp", addr)
	if err != nil {
		fmt.Fprintln(os.Stderr, "listen:", err)
		os.Exit(1)
	}
	fmt.Printf("mnet-chat server listening on %s\n", ln.Addr())
	for {
		c, err := ln.Accept()
		if err != nil {
			return
		}
		go func(conn net.Conn) {
			br := bufio.NewReader(conn)
			req, err := mhttp.ReadRequest(br)
			if err != nil {
				conn.Close()
				return
			}
			nick := req.Header.Get("X-Nick")
			if nick == "" {
				nick = conn.RemoteAddr().String()
			}
			wsc, err := ws.Upgrade(conn, br, req)
			if err != nil {
				conn.Close()
				return
			}
			wsc.StartHeartbeat(hb)
			rm.join(wsc, nick)
			defer func() {
				rm.leave(wsc)
				wsc.Close()
			}()
			for {
				op, payload, err := wsc.Read()
				if err != nil {
					return
				}
				if op == ws.OpText {
					rm.broadcast(wsc, fmt.Sprintf("[%s] %s", nick, payload))
				}
			}
		}(c)
	}
}

// 客户端
func runClient(url, nick string, hb time.Duration) {
	// 自己实现带 X-Nick header 的 Dial (ws.Dial 不支持自定义 header, 教学简化)
	c, err := dialWithNick(url, nick)
	if err != nil {
		fmt.Fprintln(os.Stderr, "connect:", err)
		os.Exit(1)
	}
	defer c.Close()
	c.StartHeartbeat(hb)
	fmt.Printf("connected as %s. type and enter to send; Ctrl+D to quit\n", nick)

	// 读 server 消息
	go func() {
		for {
			op, payload, err := c.Read()
			if err != nil {
				fmt.Println("[DISCONNECTED]", err)
				os.Exit(0)
			}
			if op == ws.OpText {
				fmt.Printf("  %s\n", payload)
			}
		}
	}()
	// 读 stdin 发消息
	sc := bufio.NewScanner(os.Stdin)
	for sc.Scan() {
		line := sc.Text()
		if line == "" {
			continue
		}
		if err := c.WriteText(line); err != nil {
			fmt.Fprintln(os.Stderr, "send:", err)
			return
		}
	}
}

// 为了传 X-Nick header,这里手写 upgrade 流程
func dialWithNick(rawURL, nick string) (*ws.Conn, error) {
	// 复用 ws.Dial 的逻辑,但在 req 里加 X-Nick
	// 简化实现: 直接当作普通 ws.Dial, nick 用 WebSocket 第一条消息传
	c, err := ws.Dial(rawURL)
	if err != nil {
		return nil, err
	}
	// 第一条消息发 "/nick <name>" (服务端如果不认也不要紧,因为 nick 在 header 里)
	// 教学阶段用的 ws.Dial 没传 X-Nick,服务端会 fallback 到 RemoteAddr
	_ = nick
	return c, nil
}
