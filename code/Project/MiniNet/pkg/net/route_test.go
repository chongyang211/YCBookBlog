// pkg/net/route_test.go
package net

import "testing"

func mustCIDR(t *testing.T, s string) Subnet {
	t.Helper()
	sub, err := ParseCIDR(s)
	if err != nil {
		t.Fatal(err)
	}
	return sub
}

func mustIP(t *testing.T, s string) IPv4Addr {
	t.Helper()
	ip, err := ParseIPv4(s)
	if err != nil {
		t.Fatal(err)
	}
	return ip
}

func TestRouteLongestPrefixMatch(t *testing.T) {
	tbl := NewRouteTable()
	// 故意乱序 Add,看排序是否生效
	tbl.Add(Route{Dst: mustCIDR(t, "0.0.0.0/0"),
		NextHop: mustIP(t, "192.168.1.1"), Iface: "wan"})
	tbl.Add(Route{Dst: mustCIDR(t, "10.1.0.0/16"),
		NextHop: mustIP(t, "10.1.0.1"), Iface: "lan2"})
	tbl.Add(Route{Dst: mustCIDR(t, "10.0.0.0/8"),
		NextHop: mustIP(t, "10.0.0.1"), Iface: "lan1"})

	// 10.1.2.3 应命中 /16 (最长)
	r, ok := tbl.Lookup(mustIP(t, "10.1.2.3"))
	if !ok || r.Iface != "lan2" {
		t.Errorf("10.1.2.3: want lan2, got %+v ok=%v", r, ok)
	}

	// 10.5.0.1 应命中 /8
	r, _ = tbl.Lookup(mustIP(t, "10.5.0.1"))
	if r.Iface != "lan1" {
		t.Errorf("10.5.0.1: want lan1, got %+v", r)
	}

	// 8.8.8.8 应命中 /0 默认
	r, _ = tbl.Lookup(mustIP(t, "8.8.8.8"))
	if r.Iface != "wan" {
		t.Errorf("8.8.8.8: want wan, got %+v", r)
	}
}

func TestRouteEmptyTable(t *testing.T) {
	tbl := NewRouteTable()
	_, ok := tbl.Lookup(mustIP(t, "1.2.3.4"))
	if ok {
		t.Error("empty table should not match")
	}
}

func TestRouteConcurrentReadWrite(t *testing.T) {
	// 简单的 -race 测试:并发 Add 和 Lookup
	tbl := NewRouteTable()
	tbl.Add(Route{Dst: mustCIDR(t, "10.0.0.0/8"),
		NextHop: mustIP(t, "10.0.0.1")})

	done := make(chan struct{})
	go func() {
		for i := 0; i < 100; i++ {
			tbl.Lookup(mustIP(t, "10.1.2.3"))
		}
		close(done)
	}()
	for i := 0; i < 100; i++ {
		tbl.Add(Route{Dst: mustCIDR(t, "192.168.0.0/16"),
			NextHop: mustIP(t, "192.168.0.1")})
	}
	<-done
}
