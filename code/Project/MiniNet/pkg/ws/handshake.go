// pkg/ws/handshake.go - WebSocket Upgrade 握手 (RFC6455 §4)
//
// 第 6 次会话 Step 14.2
//
// 客户端发:
//   GET /chat HTTP/1.1
//   Upgrade: websocket
//   Connection: Upgrade
//   Sec-WebSocket-Key: <base64 16 random bytes>
//   Sec-WebSocket-Version: 13
//
// 服务端答:
//   HTTP/1.1 101 Switching Protocols
//   Upgrade: websocket
//   Connection: Upgrade
//   Sec-WebSocket-Accept: base64(sha1(key + magic))
//
// magic = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
package ws

import (
	"bufio"
	"crypto/rand"
	"crypto/sha1"
	"encoding/base64"
	"fmt"
	"io"
	"net"
	"net/url"
	"strings"

	mhttp "mininet/pkg/http"
)

const magicGUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

// AcceptKey 计算服务端应回的 Sec-WebSocket-Accept
func AcceptKey(clientKey string) string {
	h := sha1.New()
	h.Write([]byte(clientKey + magicGUID))
	return base64.StdEncoding.EncodeToString(h.Sum(nil))
}

// Upgrade 服务端:把一个 HTTP 请求升级为 WebSocket 连接
// 注意: 这个函数不走 pkg/http 的 Server 封装,因为 Upgrade 之后要"劫持"net.Conn
// 服务端代码通常自己起 TCP listener,收到请求判断是不是 WS,再调 Upgrade
func Upgrade(conn net.Conn, br *bufio.Reader, req *mhttp.Request) (*Conn, error) {
	if !strings.EqualFold(req.Header.Get("Upgrade"), "websocket") {
		return nil, fmt.Errorf("not a websocket upgrade (Upgrade=%q)", req.Header.Get("Upgrade"))
	}
	key := req.Header.Get("Sec-WebSocket-Key")
	if key == "" {
		return nil, fmt.Errorf("missing Sec-WebSocket-Key")
	}
	accept := AcceptKey(key)

	// 回 101
	resp := "HTTP/1.1 101 Switching Protocols\r\n" +
		"Upgrade: websocket\r\n" +
		"Connection: Upgrade\r\n" +
		"Sec-WebSocket-Accept: " + accept + "\r\n" +
		"\r\n"
	if _, err := conn.Write([]byte(resp)); err != nil {
		return nil, err
	}
	return &Conn{conn: conn, br: br, isServer: true}, nil
}

// Dial 客户端:发起 WebSocket 连接 (只支持 ws://, wss:// 阶段 ⑰ 补)
func Dial(rawURL string) (*Conn, error) {
	u, err := url.Parse(rawURL)
	if err != nil {
		return nil, err
	}
	if u.Scheme != "ws" {
		return nil, fmt.Errorf("unsupported scheme: %s (only ws:// for now)", u.Scheme)
	}
	host := u.Host
	if u.Port() == "" {
		host = u.Hostname() + ":80"
	}
	conn, err := net.Dial("tcp", host)
	if err != nil {
		return nil, err
	}

	// 生成客户端 key
	rawKey := make([]byte, 16)
	rand.Read(rawKey)
	key := base64.StdEncoding.EncodeToString(rawKey)

	path := u.RequestURI()
	if path == "" {
		path = "/"
	}
	req := "GET " + path + " HTTP/1.1\r\n" +
		"Host: " + u.Hostname() + "\r\n" +
		"Upgrade: websocket\r\n" +
		"Connection: Upgrade\r\n" +
		"Sec-WebSocket-Key: " + key + "\r\n" +
		"Sec-WebSocket-Version: 13\r\n" +
		"\r\n"
	if _, err := conn.Write([]byte(req)); err != nil {
		conn.Close()
		return nil, err
	}

	br := bufio.NewReader(conn)
	resp, err := mhttp.ReadResponse(br)
	if err != nil {
		conn.Close()
		return nil, err
	}
	if resp.StatusCode != 101 {
		conn.Close()
		return nil, fmt.Errorf("upgrade failed: %d %s", resp.StatusCode, resp.Status)
	}
	// 校验 Accept
	want := AcceptKey(key)
	if resp.Header.Get("Sec-WebSocket-Accept") != want {
		conn.Close()
		return nil, fmt.Errorf("Sec-WebSocket-Accept mismatch")
	}
	return &Conn{conn: conn, br: br, isServer: false}, nil
}

// 保证 io 包不被 goimports 删
var _ io.Reader = (*bufio.Reader)(nil)
