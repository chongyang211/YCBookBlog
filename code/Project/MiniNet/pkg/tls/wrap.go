// pkg/tls/wrap.go - 包装 crypto/tls 的 Dial + 握手观察钩子
//
// 第 5 次会话 Step 10.3
//
// 实际 TLS 加解密由标准库 crypto/tls 完成(自己写一次 X25519+AES-GCM 要 ~1000 行).
// 我们只做:
//   1. 用标准库 Dial,拿到 ConnectionState 可视化 (版本/套件/证书链/ALPN)
//   2. 在 Dial 前先截一个 ClientHello 字节,交给我们自己的解析器打印
//
// 这样"真·端到端 TLS"和"教学·字节分布"两头都占.
package tls

import (
	"crypto/tls"
	"crypto/x509"
	"errors"
	"fmt"
	"io"
	"net"
	"strings"
	"time"
)

// DialInfo 一次 TLS 握手的可视化结果
type DialInfo struct {
	ServerName       string
	Version          ProtoVersion
	CipherSuite      uint16
	NegotiatedProto  string              // ALPN 协商结果
	PeerCerts        []*x509.Certificate // 证书链 (服务器 → 中间 → 根)
	HandshakeTime    time.Duration
}

// String 格式化 (给 mnet-curl --tls-debug 用)
func (d *DialInfo) String() string {
	var b strings.Builder
	fmt.Fprintf(&b, "TLS Connection:\n")
	fmt.Fprintf(&b, "  Server            : %s\n", d.ServerName)
	fmt.Fprintf(&b, "  Version           : %s\n", d.Version)
	fmt.Fprintf(&b, "  Cipher            : %s\n", CipherSuiteName(d.CipherSuite))
	if d.NegotiatedProto != "" {
		fmt.Fprintf(&b, "  ALPN              : %s\n", d.NegotiatedProto)
	}
	fmt.Fprintf(&b, "  Handshake Time    : %v\n", d.HandshakeTime)
	fmt.Fprintf(&b, "  Certificate Chain : %d levels\n", len(d.PeerCerts))
	for i, c := range d.PeerCerts {
		marker := "└"
		if i < len(d.PeerCerts)-1 {
			marker = "├"
		}
		fmt.Fprintf(&b, "  %s [%d] Subject   : %s\n", marker, i, c.Subject)
		fmt.Fprintf(&b, "       Issuer     : %s\n", c.Issuer)
		fmt.Fprintf(&b, "       NotBefore  : %s\n", c.NotBefore.Format("2006-01-02"))
		fmt.Fprintf(&b, "       NotAfter   : %s\n", c.NotAfter.Format("2006-01-02"))
		if len(c.DNSNames) > 0 && len(c.DNSNames) <= 5 {
			fmt.Fprintf(&b, "       DNS Names  : %v\n", c.DNSNames)
		} else if len(c.DNSNames) > 5 {
			fmt.Fprintf(&b, "       DNS Names  : %v (and %d more)\n",
				c.DNSNames[:3], len(c.DNSNames)-3)
		}
	}
	return b.String()
}

// Dial 对 addr 发起 TLS 连接,返回连接 + 可视化信息
// addr: "example.com:443"
// serverName: SNI (通常同 host)
// alpn: 想协商的协议列表,如 []string{"h2", "http/1.1"}
//
// 返回的 net.Conn 已完成握手,可以直接 Write/Read 走明文(crypto/tls 替你加密)
func Dial(addr, serverName string, alpn []string, insecureSkipVerify bool) (net.Conn, *DialInfo, error) {
	conf := &tls.Config{
		ServerName:         serverName,
		NextProtos:         alpn,
		InsecureSkipVerify: insecureSkipVerify, //nolint:gosec  // 教学 demo 允许
	}

	t0 := time.Now()
	conn, err := tls.Dial("tcp", addr, conf)
	if err != nil {
		return nil, nil, fmt.Errorf("tls dial %s: %w", addr, err)
	}
	elapsed := time.Since(t0)

	state := conn.ConnectionState()
	info := &DialInfo{
		ServerName:      state.ServerName,
		Version:         ProtoVersion(state.Version),
		CipherSuite:     state.CipherSuite,
		NegotiatedProto: state.NegotiatedProtocol,
		PeerCerts:       state.PeerCertificates,
		HandshakeTime:   elapsed,
	}
	return conn, info, nil
}

// CaptureClientHello 构造一个"真·crypto/tls 发出的 ClientHello"字节,
// 用我们自己的 ParseClientHello 解析 + Describe 打印.
//
// 实现:在本地起一个 TCP server,把它发的第一个 TLS record 拦下来 → 解析.
// 不做真实握手(会报错,因为 server 不是真 TLS),拦下 ClientHello 后立刻关连接.
func CaptureClientHello(serverName string, alpn []string) (*ClientHello, []byte, error) {
	// 本地 TCP server 作为"伪 TLS 服务端"
	ln, err := net.Listen("tcp", "127.0.0.1:0")
	if err != nil {
		return nil, nil, err
	}
	defer ln.Close()

	addr := ln.Addr().String()
	type result struct {
		buf []byte
		err error
	}
	ch := make(chan result, 1)

	go func() {
		c, err := ln.Accept()
		if err != nil {
			ch <- result{err: err}
			return
		}
		defer c.Close()
		c.SetDeadline(time.Now().Add(3 * time.Second))
		// 读 ClientHello: TLS record 头 5 字节 → 读 length 字段 → 再读完
		head := make([]byte, 5)
		if _, err := io.ReadFull(c, head); err != nil {
			ch <- result{err: fmt.Errorf("read record head: %w", err)}
			return
		}
		plen := int(head[3])<<8 | int(head[4])
		body := make([]byte, plen)
		if _, err := io.ReadFull(c, body); err != nil {
			ch <- result{err: fmt.Errorf("read record body: %w", err)}
			return
		}
		full := append(head, body...)
		ch <- result{buf: full}
	}()

	// crypto/tls 发起握手,它会卡在"等 ServerHello"直到超时/我们关闭
	// 用 goroutine 发起,不关心它的结果
	go func() {
		conf := &tls.Config{
			ServerName:         serverName,
			NextProtos:         alpn,
			InsecureSkipVerify: true,
		}
		conn, err := tls.DialWithDialer(
			&net.Dialer{Timeout: 3 * time.Second},
			"tcp", addr, conf,
		)
		if err == nil {
			conn.Close()
		}
	}()

	select {
	case r := <-ch:
		if r.err != nil {
			return nil, nil, r.err
		}
		// 解析:head 是 TLS record, record.Payload 第 1 字节是 HandshakeType=1 (ClientHello),
		// 后 3 字节是 handshake body 长度
		rec, _, err := DecodeRecord(r.buf)
		if err != nil {
			return nil, nil, err
		}
		if rec.Type != RecTypeHandshake {
			return nil, nil, fmt.Errorf("not handshake, got %s", rec.Type)
		}
		if len(rec.Payload) < 4 {
			return nil, nil, errors.New("handshake header too short")
		}
		if HandshakeType(rec.Payload[0]) != HSTypeClientHello {
			return nil, nil, fmt.Errorf("not ClientHello, got %s", HandshakeType(rec.Payload[0]))
		}
		bodyLen := int(rec.Payload[1])<<16 | int(rec.Payload[2])<<8 | int(rec.Payload[3])
		if 4+bodyLen > len(rec.Payload) {
			return nil, nil, fmt.Errorf("ClientHello body truncated: want %d have %d", bodyLen, len(rec.Payload)-4)
		}
		body := rec.Payload[4 : 4+bodyLen]
		hello, err := ParseClientHello(body)
		if err != nil {
			return nil, nil, err
		}
		return hello, r.buf, nil
	case <-time.After(5 * time.Second):
		return nil, nil, errors.New("capture timeout")
	}
}
