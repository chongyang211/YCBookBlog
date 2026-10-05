// pkg/dns/resolver_test.go - 本地假 DNS server 的端到端测试
// 不依赖公网
package dns

import (
	"context"
	"net"
	"strconv"
	"sync/atomic"
	"testing"
	"time"
)

// startFakeDNS 开一个本地 UDP:53 fake server
// 对任何 A 查询都返回固定 IP
// 返回 (addr, 关闭函数, 收到查询次数指针)
func startFakeDNS(t *testing.T, answerIP net.IP) (string, func(), *atomic.Int64) {
	t.Helper()
	conn, err := net.ListenUDP("udp", &net.UDPAddr{IP: net.IPv4(127, 0, 0, 1)})
	if err != nil {
		t.Fatal(err)
	}
	addr := conn.LocalAddr().(*net.UDPAddr)

	var queryN atomic.Int64
	go func() {
		buf := make([]byte, 1500)
		for {
			n, raddr, err := conn.ReadFromUDP(buf)
			if err != nil {
				return
			}
			queryN.Add(1)
			msg, err := Decode(buf[:n])
			if err != nil || len(msg.Questions) == 0 {
				continue
			}
			q := msg.Questions[0]
			// 构造 response
			msg.Header.Flags = 1<<15 | 1<<7 // QR=1, RA=1
			msg.Header.ANCount = 1
			msg.Answers = []RR{{
				Name:  q.Name,
				Type:  TypeA,
				Class: ClassIN,
				TTL:   60,
				Data:  answerIP.To4(),
			}}
			// Encode: 只写 header + question + 一个 Answer (手工)
			out, _ := msg.Encode()
			// 追加 answer: name 用压缩指针 (指向 question 的 name,偏移 12)
			out = append(out, 0xC0, 0x0C)
			out = append(out, byte(TypeA>>8), byte(TypeA))
			out = append(out, 0, 1) // class IN
			out = append(out, 0, 0, 0, 60) // ttl
			out = append(out, 0, 4) // rdlen=4
			out = append(out, answerIP.To4()...)
			conn.WriteToUDP(out, raddr)
		}
	}()
	return "127.0.0.1:" + strconv.Itoa(addr.Port), func() { conn.Close() }, &queryN
}

func TestQueryServerAgainstFakeDNS(t *testing.T) {
	addr, cleanup, n := startFakeDNS(t, net.IPv4(93, 184, 216, 34))
	defer cleanup()

	ctx, cancel := context.WithTimeout(context.Background(), 2*time.Second)
	defer cancel()

	msg, err := QueryServer(ctx, addr, "www.example.com", TypeA)
	if err != nil {
		t.Fatal(err)
	}
	if len(msg.Answers) == 0 {
		t.Fatal("no answers")
	}
	ip := net.IP(msg.Answers[0].Data)
	if ip.String() != "93.184.216.34" {
		t.Errorf("got %s, want 93.184.216.34", ip)
	}
	if n.Load() != 1 {
		t.Errorf("server got %d queries, want 1", n.Load())
	}
}

func TestResolverCacheAndSingleflight(t *testing.T) {
	addr, cleanup, serverQueryN := startFakeDNS(t, net.IPv4(1, 2, 3, 4))
	defer cleanup()

	// 临时把 rootServers 指向我们的 fake server (只影响本测试进程内的递归入口)
	oldRoots := rootServers
	rootServers = []string{addr}
	defer func() { rootServers = oldRoots }()

	r := NewResolver(2*time.Second, 10*time.Second)
	ctx := context.Background()

	// 第一次 Resolve:应该打一次 fake server
	ips, err := r.Resolve(ctx, "example.com")
	if err != nil {
		t.Fatal(err)
	}
	if len(ips) == 0 || ips[0].String() != "1.2.3.4" {
		t.Fatalf("ips = %v", ips)
	}
	n1 := serverQueryN.Load()

	// 第二次 Resolve:应该走缓存,fake server 查询数不变
	ips2, _ := r.Resolve(ctx, "example.com")
	if ips2[0].String() != "1.2.3.4" {
		t.Error("cache hit value wrong")
	}
	if serverQueryN.Load() != n1 {
		t.Errorf("cache miss: before=%d after=%d", n1, serverQueryN.Load())
	}
	t.Logf("✅ cache hit, server stayed at %d queries", n1)
}
