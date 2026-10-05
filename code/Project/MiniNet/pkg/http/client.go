// pkg/http/client.go - HTTP/1.1 客户端
//
// 第 5 次会话 Step 11.5
//
// 支持:
//   - http:// 走 net.Dial
//   - https:// 走我们自己的 pkg/tls (包装 crypto/tls)
//   - -v verbose: 打印 req/resp 的 header
package http

import (
	"bufio"
	"errors"
	"fmt"
	"io"
	"net"
	"net/url"
	"strings"
	"time"

	mtls "mininet/pkg/tls"
)

// Client HTTP/1.1 客户端
type Client struct {
	Timeout time.Duration
	Verbose bool // -v 打印 req/resp header
	// TLSDebug 为 true 时打印 ClientHello 字节分析
	TLSDebug bool
	// InsecureSkipVerify 跳过证书校验 (教学 demo 可用)
	InsecureSkipVerify bool
}

// Result 一次请求的完整结果
type Result struct {
	Request  *Request
	Response *Response
	TLSInfo  *mtls.DialInfo // 仅 HTTPS
	Elapsed  time.Duration
}

// Get 便捷方法
func (c *Client) Get(rawURL string) (*Result, error) {
	return c.Do("GET", rawURL, nil, nil)
}

// Do 发起一次请求
func (c *Client) Do(method, rawURL string, headers Header, body []byte) (*Result, error) {
	u, err := url.Parse(rawURL)
	if err != nil {
		return nil, fmt.Errorf("parse url: %w", err)
	}
	if u.Scheme != "http" && u.Scheme != "https" {
		return nil, fmt.Errorf("unsupported scheme: %s", u.Scheme)
	}

	host := u.Host
	port := u.Port()
	if port == "" {
		if u.Scheme == "https" {
			port = "443"
		} else {
			port = "80"
		}
		host = u.Hostname() + ":" + port
	}

	path := u.RequestURI()
	if path == "" {
		path = "/"
	}

	req := &Request{
		Method: method,
		URL:    path,
		Proto:  "HTTP/1.1",
		Header: make(Header),
		Body:   body,
	}
	req.Header.Set("Host", u.Hostname())
	req.Header.Set("User-Agent", "mnet-curl/0.5")
	req.Header.Set("Accept", "*/*")
	req.Header.Set("Connection", "close")
	for k, v := range headers {
		req.Header.Set(k, v)
	}

	t0 := time.Now()
	var conn net.Conn
	var tlsInfo *mtls.DialInfo
	timeout := c.Timeout
	if timeout == 0 {
		timeout = 10 * time.Second
	}

	if u.Scheme == "https" {
		conn, tlsInfo, err = mtls.Dial(host, u.Hostname(), []string{"http/1.1"}, c.InsecureSkipVerify)
		if err != nil {
			return nil, err
		}
	} else {
		conn, err = net.DialTimeout("tcp", host, timeout)
		if err != nil {
			return nil, err
		}
	}
	defer conn.Close()
	conn.SetDeadline(time.Now().Add(timeout))

	// 发请求
	if c.Verbose {
		fmt.Printf("> %s %s %s\n", req.Method, req.URL, req.Proto)
		for k, v := range req.Header {
			fmt.Printf("> %s: %s\n", k, v)
		}
		fmt.Println(">")
	}
	if _, err := req.WriteTo(conn); err != nil {
		return nil, fmt.Errorf("write req: %w", err)
	}

	// 读响应
	br := bufio.NewReader(conn)
	resp, err := ReadResponse(br)
	if err != nil && !errors.Is(err, io.EOF) {
		return nil, fmt.Errorf("read resp: %w", err)
	}
	if c.Verbose && resp != nil {
		fmt.Printf("< %s %s\n", resp.Proto, resp.Status)
		for k, v := range resp.Header {
			fmt.Printf("< %s: %s\n", k, v)
		}
		fmt.Println("<")
	}

	elapsed := time.Since(t0)
	return &Result{
		Request:  req,
		Response: resp,
		TLSInfo:  tlsInfo,
		Elapsed:  elapsed,
	}, nil
}

// CleanURL 去掉默认端口 (给打印用)
func CleanURL(u string) string {
	return strings.TrimSuffix(strings.TrimSuffix(u, ":443"), ":80")
}
