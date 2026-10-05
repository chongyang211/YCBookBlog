// pkg/net/l3_test.go - 端到端:两个 peer 互相 ping
// 用外部测试包 net_test 避免 import cycle (pkg/link 已 import pkg/net)
package net_test

import (
	"testing"
	"time"

	"mininet/pkg/link"
	netpkg "mininet/pkg/net"
)

// buildPair 返回两个 L3 配好的 peer + 清理函数
func buildPair(t *testing.T, ipA, ipB string) (*netpkg.L3Layer, *netpkg.L3Layer, func()) {
	t.Helper()
	drvA, drvB := link.LoopbackPair(
		link.MustParseMAC("aa:bb:cc:dd:ee:01"),
		link.MustParseMAC("aa:bb:cc:dd:ee:02"),
	)
	ipAddrA, _ := netpkg.ParseIPv4(ipA)
	ipAddrB, _ := netpkg.ParseIPv4(ipB)
	l2A := link.NewL2Layer(drvA, ipAddrA)
	l2B := link.NewL2Layer(drvB, ipAddrB)
	l3A := netpkg.NewL3Layer(l2A, netpkg.NewRouteTable())
	l3B := netpkg.NewL3Layer(l2B, netpkg.NewRouteTable())
	cleanup := func() {
		l2A.Close()
		l2B.Close()
	}
	return l3A, l3B, cleanup
}

func TestEndToEndPing(t *testing.T) {
	l3A, _, cleanup := buildPair(t, "10.0.0.1", "10.0.0.2")
	defer cleanup()

	ipB, _ := netpkg.ParseIPv4("10.0.0.2")

	// 首次 ping 会先 ARP(~ms),应在 500ms 内收到 reply
	for seq := uint16(1); seq <= 3; seq++ {
		r := l3A.Ping(ipB, 42, seq, 500*time.Millisecond)
		if r.Timeout {
			t.Fatalf("seq=%d timeout", seq)
		}
		if r.Err != nil {
			t.Fatalf("seq=%d err: %v", seq, r.Err)
		}
		t.Logf("ping result: %s", r.String())
	}
}

func TestPingTimeout(t *testing.T) {
	// 只建 A,没有 peer → ping 应超时或 ARP 失败
	drvA, _ := link.LoopbackPair(
		link.MustParseMAC("aa:bb:cc:dd:ee:01"),
		link.MAC{},
	)
	ipA, _ := netpkg.ParseIPv4("10.0.0.1")
	l2A := link.NewL2Layer(drvA, ipA)
	defer l2A.Close()
	l3A := netpkg.NewL3Layer(l2A, netpkg.NewRouteTable())

	ipB, _ := netpkg.ParseIPv4("10.0.0.99")
	// ARP 超时是 3s,但 Ping 的 timeout 更长时等 ARP 回错误;
	// 这里给 5s,预期 ~3s 后返回 Err
	r := l3A.Ping(ipB, 1, 1, 5*time.Second)
	if !r.Timeout && r.Err == nil {
		t.Fatalf("expect timeout or err, got reply ttl=%d", r.TTL)
	}
	t.Logf("expected failure: %s", r.String())
}
