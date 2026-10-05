// tests/dns_demo/main.go - DNS 本地演示 (不依赖公网)
//
// 跑法:
//   go run ./tests/dns_demo
//   # 或
//   make dns-demo
//
// 启动一个本地 fake DNS server (:5353), 然后用 mnet-dig 的客户端 API 查三次,
// 验证缓存 + singleflight.
package main

import (
	"context"
	"fmt"
	"net"
	"sync"
	"sync/atomic"
	"time"

	"mininet/pkg/common"
	"mininet/pkg/dns"
)

func main() {
	common.SetLogLevel(common.LvInfo)

	// 启动本地 fake DNS
	addr, cleanup, serverN := startFakeDNS()
	defer cleanup()

	fmt.Printf("====== fake DNS listening @ %s ======\n\n", addr)

	// === 测试 1: 直接查 ===
	fmt.Println("[1] 直接 QueryServer")
	ctx := context.Background()
	t0 := time.Now()
	msg, err := dns.QueryServer(ctx, addr, "www.example.com", dns.TypeA)
	if err != nil {
		fmt.Println("err:", err)
		return
	}
	fmt.Printf("    answer: %s (耗时 %v)\n", formatAnswer(msg), time.Since(t0))
	fmt.Printf("    server 收到查询数: %d\n\n", serverN.Load())

	// === 测试 2: 用 Resolver 查两次(第二次应该走缓存) ===
	fmt.Println("[2] Resolver 查两次(验证缓存)")
	r := dns.NewResolver(2*time.Second, 10*time.Second)
	// 把 fake server 当"根"注入
	r.Roots = []string{addr}

	before := serverN.Load()
	for i := 1; i <= 2; i++ {
		t0 := time.Now()
		ips, err := r.Resolve(ctx, "www.example.com")
		if err != nil {
			fmt.Printf("    #%d err: %v\n", i, err)
			continue
		}
		fmt.Printf("    #%d: %v (耗时 %v)\n", i, ips, time.Since(t0))
	}
	fmt.Printf("    第二次查询应走缓存: server 新增查询 %d (期望 1)\n\n",
		serverN.Load()-before)

	// === 测试 3: 100 并发同域名(验证 singleflight) ===
	fmt.Println("[3] 100 并发查新域名(验证 singleflight)")
	before = serverN.Load()
	var wg sync.WaitGroup
	t0 = time.Now()
	for i := 0; i < 100; i++ {
		wg.Add(1)
		go func() {
			defer wg.Done()
			r.Resolve(ctx, "sf-test.example.com")
		}()
	}
	wg.Wait()
	fmt.Printf("    100 goroutine 并发查同域名,耗时 %v\n", time.Since(t0))
	fmt.Printf("    server 新增查询 %d (期望 1 — singleflight 救场)\n\n",
		serverN.Load()-before)

	fmt.Println("====== 📊 结论 ======")
	fmt.Println("DNS 缓存 + singleflight 是生产 DNS client 的标配:")
	fmt.Println("  - 没缓存: 每次 RTT ~50-200ms,源站 QPS 爆炸")
	fmt.Println("  - 有缓存: 命中后 <1μs,源站 QPS 几乎 0")
	fmt.Println("  - singleflight: 1000 并发查新域名 = 1 次真查询")
}

// -------- 本地 fake DNS server --------
var fakeIP = net.IPv4(93, 184, 216, 34)

func startFakeDNS() (string, func(), *atomic.Int64) {
	conn, err := net.ListenUDP("udp", &net.UDPAddr{IP: net.IPv4(127, 0, 0, 1)})
	if err != nil {
		panic(err)
	}
	addr := conn.LocalAddr().String()
	var n atomic.Int64
	go func() {
		buf := make([]byte, 1500)
		for {
			nb, raddr, err := conn.ReadFromUDP(buf)
			if err != nil {
				return
			}
			n.Add(1)
			msg, err := dns.Decode(buf[:nb])
			if err != nil || len(msg.Questions) == 0 {
				continue
			}
			q := msg.Questions[0]
			// 构造 response: 用 Message.Answers + Encode 让 Encode 自动处理
			resp := &dns.Message{}
			resp.Header = msg.Header
			resp.Header.Flags = 1<<15 | 1<<7 // QR=1 RA=1
			resp.Questions = msg.Questions
			resp.Answers = []dns.RR{{
				Name:  q.Name,
				Type:  dns.TypeA,
				Class: dns.ClassIN,
				TTL:   60,
				Data:  fakeIP.To4(),
			}}
			out, _ := resp.Encode()
			conn.WriteToUDP(out, raddr)
		}
	}()
	return addr, func() { conn.Close() }, &n
}

func formatAnswer(m *dns.Message) string {
	for _, rr := range m.Answers {
		if rr.Type == dns.TypeA && len(rr.Data) == 4 {
			return net.IP(rr.Data).String()
		}
	}
	return "(no A record)"
}
