// pkg/transport/e2e_test.go - 端到端:两 peer 的 TCP 握手挥手 + 数据传输
// 用外部测试包避免 import cycle
package transport_test

import (
	"bytes"
	"io"
	"testing"
	"time"

	"mininet/pkg/link"
	netpkg "mininet/pkg/net"
	"mininet/pkg/transport"
)

// buildPair 搭两 peer + L2 + L3 + L4
func buildPair(t *testing.T) (*transport.L4Layer, *transport.L4Layer, func()) {
	t.Helper()
	drvA, drvB := link.LoopbackPair(
		link.MustParseMAC("aa:bb:cc:dd:ee:01"),
		link.MustParseMAC("aa:bb:cc:dd:ee:02"),
	)
	ipA, _ := netpkg.ParseIPv4("10.0.0.1")
	ipB, _ := netpkg.ParseIPv4("10.0.0.2")
	l2A := link.NewL2Layer(drvA, ipA)
	l2B := link.NewL2Layer(drvB, ipB)
	l3A := netpkg.NewL3Layer(l2A, netpkg.NewRouteTable())
	l3B := netpkg.NewL3Layer(l2B, netpkg.NewRouteTable())
	l4A := transport.NewL4Layer(l3A)
	l4B := transport.NewL4Layer(l3B)
	cleanup := func() {
		l4A.Close()
		l4B.Close()
		l2A.Close()
		l2B.Close()
	}
	return l4A, l4B, cleanup
}

// TestHandshake 三次握手走通
func TestHandshake(t *testing.T) {
	l4A, l4B, cleanup := buildPair(t)
	defer cleanup()

	// B 监听 :8080
	ln, err := l4B.Listen(8080)
	if err != nil {
		t.Fatal(err)
	}

	// 启动 accept goroutine
	accepted := make(chan *transport.Conn, 1)
	go func() {
		c, err := ln.Accept()
		if err != nil {
			t.Errorf("accept: %v", err)
			return
		}
		accepted <- c
	}()

	// A 发起 Dial
	ipB, _ := netpkg.ParseIPv4("10.0.0.2")
	time.Sleep(10 * time.Millisecond) // 等 listen 生效
	ca, err := l4A.Dial(ipB, 8080, 2*time.Second)
	if err != nil {
		t.Fatalf("dial: %v", err)
	}

	cb := <-accepted
	if ca.State() != transport.StateEstablished || cb.State() != transport.StateEstablished {
		t.Fatalf("not established: A=%s B=%s", ca.State(), cb.State())
	}
	t.Logf("A port=%d state=%s | B port=%d state=%s",
		ca.LocalPort(), ca.State(), cb.LocalPort(), cb.State())
}

// TestDataTransfer 握手后互发数据
func TestDataTransfer(t *testing.T) {
	l4A, l4B, cleanup := buildPair(t)
	defer cleanup()

	ln, _ := l4B.Listen(8081)
	// server echo
	go func() {
		c, err := ln.Accept()
		if err != nil {
			return
		}
		defer c.Close()
		buf := make([]byte, 4096)
		for {
			n, err := c.Read(buf)
			if n > 0 {
				c.Write(buf[:n])
			}
			if err != nil {
				return
			}
		}
	}()

	ipB, _ := netpkg.ParseIPv4("10.0.0.2")
	time.Sleep(10 * time.Millisecond)
	ca, err := l4A.Dial(ipB, 8081, 2*time.Second)
	if err != nil {
		t.Fatal(err)
	}

	// 发消息
	msg := []byte("hello mnet TCP!")
	if _, err := ca.Write(msg); err != nil {
		t.Fatal(err)
	}
	// 读 echo
	buf := make([]byte, 4096)
	n, err := ca.Read(buf)
	if err != nil {
		t.Fatal(err)
	}
	if !bytes.Equal(buf[:n], msg) {
		t.Errorf("echo mismatch: got %q want %q", buf[:n], msg)
	}
	t.Logf("echo roundtrip ok: %q", buf[:n])

	// 关闭
	ca.Close()
}

// TestFourWayClose 四次挥手走通
func TestFourWayClose(t *testing.T) {
	l4A, l4B, cleanup := buildPair(t)
	defer cleanup()

	ln, _ := l4B.Listen(8082)
	serverGotEOF := make(chan struct{})
	go func() {
		c, err := ln.Accept()
		if err != nil {
			return
		}
		buf := make([]byte, 4096)
		for {
			_, err := c.Read(buf)
			if err == io.EOF {
				close(serverGotEOF)
				c.Close() // server 侧也 close → LAST_ACK
				return
			}
			if err != nil {
				return
			}
		}
	}()

	ipB, _ := netpkg.ParseIPv4("10.0.0.2")
	time.Sleep(10 * time.Millisecond)
	ca, err := l4A.Dial(ipB, 8082, 2*time.Second)
	if err != nil {
		t.Fatal(err)
	}

	// A 主动关闭
	if err := ca.Close(); err != nil {
		t.Fatal(err)
	}

	// server 应收到 EOF
	select {
	case <-serverGotEOF:
	case <-time.After(2 * time.Second):
		t.Fatal("server didn't see EOF")
	}

	// A 应在 2*MSL 后进 CLOSED
	done := make(chan struct{})
	go func() { ca.WaitClosed(); close(done) }()
	select {
	case <-done:
		t.Logf("A reached CLOSED after 2*MSL")
	case <-time.After(3 * time.Second):
		t.Fatal("A didn't reach CLOSED after 2*MSL")
	}
}

// TestLargeTransfer 验证按 MSS 分片发大 payload
func TestLargeTransfer(t *testing.T) {
	l4A, l4B, cleanup := buildPair(t)
	defer cleanup()

	ln, _ := l4B.Listen(8083)
	go func() {
		c, _ := ln.Accept()
		defer c.Close()
		io.Copy(io.Discard, c) // 一直读到 EOF
	}()

	ipB, _ := netpkg.ParseIPv4("10.0.0.2")
	time.Sleep(10 * time.Millisecond)
	ca, err := l4A.Dial(ipB, 8083, 2*time.Second)
	if err != nil {
		t.Fatal(err)
	}

	// 发 5 KB (约 5 个 MSS)
	payload := bytes.Repeat([]byte{0xAB}, 5000)
	n, err := ca.Write(payload)
	if err != nil {
		t.Fatal(err)
	}
	if n != len(payload) {
		t.Errorf("wrote %d, want %d", n, len(payload))
	}
	ca.Close()
	t.Logf("sent %d bytes across ~%d MSS segments", n, (n+transport.MSS-1)/transport.MSS)
}
