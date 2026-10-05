// pkg/net/subnet_test.go
package net

import "testing"

func TestIPv4ParseAndString(t *testing.T) {
	cases := []struct {
		in  string
		raw uint32
	}{
		{"0.0.0.0", 0x00000000},
		{"10.0.0.1", 0x0A000001},
		{"192.168.1.100", 0xC0A80164},
		{"255.255.255.255", 0xFFFFFFFF},
	}
	for _, c := range cases {
		ip, err := ParseIPv4(c.in)
		if err != nil {
			t.Fatalf("Parse %q: %v", c.in, err)
		}
		if uint32(ip) != c.raw {
			t.Errorf("Parse %q: got 0x%08X, want 0x%08X",
				c.in, uint32(ip), c.raw)
		}
		// 环回测试
		if ip.String() != c.in {
			t.Errorf("String %08X: got %q, want %q",
				uint32(ip), ip.String(), c.in)
		}
	}
}

func TestIPv4ParseError(t *testing.T) {
	bads := []string{"", "10.0.0", "10.0.0.0.0", "10.0.0.256", "a.b.c.d"}
	for _, b := range bads {
		if _, err := ParseIPv4(b); err == nil {
			t.Errorf("expect error for %q", b)
		}
	}
}

func TestSubnetContains(t *testing.T) {
	s, err := ParseCIDR("10.0.0.0/8")
	if err != nil {
		t.Fatal(err)
	}

	mustIn := []string{"10.0.0.0", "10.0.0.1", "10.1.2.3", "10.255.255.255"}
	for _, ip := range mustIn {
		x, _ := ParseIPv4(ip)
		if !s.Contains(x) {
			t.Errorf("%s should be in %s", ip, s)
		}
	}

	mustOut := []string{"11.0.0.0", "9.255.255.255", "192.168.1.1"}
	for _, ip := range mustOut {
		x, _ := ParseIPv4(ip)
		if s.Contains(x) {
			t.Errorf("%s should NOT be in %s", ip, s)
		}
	}
}

func TestSubnetParseEdgeCases(t *testing.T) {
	// /0 匹配所有 IP (默认路由)
	s, _ := ParseCIDR("0.0.0.0/0")
	x, _ := ParseIPv4("8.8.8.8")
	if !s.Contains(x) {
		t.Error("/0 should contain any IP")
	}
	// /32 只匹配自己
	s, _ = ParseCIDR("10.0.0.5/32")
	x, _ = ParseIPv4("10.0.0.5")
	if !s.Contains(x) {
		t.Error("/32 should contain itself")
	}
	x, _ = ParseIPv4("10.0.0.6")
	if s.Contains(x) {
		t.Error("/32 should not contain neighbor")
	}
}
