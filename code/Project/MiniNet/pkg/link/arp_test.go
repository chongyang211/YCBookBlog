package link

import (
	"testing"
	"time"

	netpkg "mininet/pkg/net"
)

func mustIP(t *testing.T, s string) netpkg.IPv4Addr {
	t.Helper()
	ip, err := netpkg.ParseIPv4(s)
	if err != nil {
		t.Fatal(err)
	}
	return ip
}

func TestArpEncodeDecode(t *testing.T) {
	p := &ArpPacket{Op: ArpOpRequest}
	p.Sender.MAC = MustParseMAC("aa:bb:cc:dd:ee:01")
	p.Sender.IP = mustIP(t, "10.0.0.1")
	p.Target.MAC = MAC{} // 请求时为 0
	p.Target.IP = mustIP(t, "10.0.0.2")

	buf := p.Encode()
	if len(buf) != ArpPacketSize {
		t.Fatalf("size = %d, want 28", len(buf))
	}
	// HTYPE
	if buf[0] != 0 || buf[1] != 1 {
		t.Error("htype wrong")
	}
	// PTYPE
	if buf[2] != 0x08 || buf[3] != 0x00 {
		t.Error("ptype wrong")
	}
	// opcode
	if buf[6] != 0 || buf[7] != 1 {
		t.Error("opcode wrong")
	}

	p2, err := DecodeArp(buf)
	if err != nil {
		t.Fatal(err)
	}
	if p2.Op != p.Op || p2.Sender.MAC != p.Sender.MAC ||
		p2.Sender.IP != p.Sender.IP || p2.Target.IP != p.Target.IP {
		t.Errorf("roundtrip mismatch: %+v vs %+v", p, p2)
	}
}

func TestArpTableInsertLookup(t *testing.T) {
	tbl := NewArpTable()
	defer tbl.Close()

	ip := mustIP(t, "10.0.0.2")
	mac := MustParseMAC("aa:bb:cc:dd:ee:02")

	// 初始空
	if _, ok := tbl.Lookup(ip); ok {
		t.Error("empty table lookup should miss")
	}
	tbl.Insert(ip, mac)
	got, ok := tbl.Lookup(ip)
	if !ok || got != mac {
		t.Errorf("want %s true, got %s %v", mac, got, ok)
	}
}

func TestArpTableWaitFor(t *testing.T) {
	tbl := NewArpTable()
	defer tbl.Close()

	ip := mustIP(t, "10.0.0.2")
	mac := MustParseMAC("aa:bb:cc:dd:ee:02")

	ch := tbl.WaitFor(ip)
	go func() {
		time.Sleep(20 * time.Millisecond)
		tbl.Insert(ip, mac)
	}()
	select {
	case got := <-ch:
		if got != mac {
			t.Errorf("waiter got %s want %s", got, mac)
		}
	case <-time.After(500 * time.Millisecond):
		t.Fatal("waiter not woken by Insert")
	}
}

func TestL2LayerArpResolve(t *testing.T) {
	// 两个 peer 互连:A 问 B 的 MAC
	driverA, driverB := LoopbackPair(
		MustParseMAC("aa:bb:cc:dd:ee:01"),
		MustParseMAC("aa:bb:cc:dd:ee:02"),
	)
	lA := NewL2Layer(driverA, mustIP(t, "10.0.0.1"))
	lB := NewL2Layer(driverB, mustIP(t, "10.0.0.2"))
	defer lA.Close()
	defer lB.Close()

	mac, err := lA.Resolve(mustIP(t, "10.0.0.2"))
	if err != nil {
		t.Fatalf("resolve: %v", err)
	}
	if mac != driverB.MAC() {
		t.Errorf("resolved MAC = %s, want %s", mac, driverB.MAC())
	}

	// 第二次 lookup 应命中缓存 (不走广播)
	mac2, err := lA.Resolve(mustIP(t, "10.0.0.2"))
	if err != nil || mac2 != mac {
		t.Error("second resolve should hit cache")
	}
}

func TestL2LayerArpTimeout(t *testing.T) {
	// 只建 A,没有 peer,resolve 应超时
	// 为加速测试,把超时常量临时设短
	drvA, _ := LoopbackPair(MustParseMAC("aa:bb:cc:dd:ee:01"), MAC{})
	lA := NewL2Layer(drvA, mustIP(t, "10.0.0.1"))
	defer lA.Close()

	// 单测期望:3s 内返回错误 (ArpResolveTimeout)
	start := time.Now()
	_, err := lA.Resolve(mustIP(t, "10.0.0.99"))
	if err == nil {
		t.Fatal("expect timeout error")
	}
	if time.Since(start) < ArpResolveTimeout {
		t.Errorf("returned too early: %v", time.Since(start))
	}
}
