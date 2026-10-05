// pkg/common/bigendian_test.go
package common

import "testing"

func TestBeU16(t *testing.T) {
	var b BeU16
	b.Set(20)
	// 内存必须是 00 14 (大端)
	if b[0] != 0x00 || b[1] != 0x14 {
		t.Errorf("Set(20): got bytes %02x %02x, want 00 14", b[0], b[1])
	}
	if b.Get() != 20 {
		t.Errorf("Get: got %d, want 20", b.Get())
	}

	// 边界值
	b.Set(0xFFFF)
	if b.Get() != 0xFFFF {
		t.Error("Get after Set(0xFFFF) failed")
	}

	// 从字节数组直接构造(模拟从网线接收)
	raw := [2]byte{0x14, 0x00}
	b2 := BeU16(raw)
	if b2.Get() != 0x1400 {
		t.Errorf("direct raw: got 0x%04X, want 0x1400", b2.Get())
	}
}

func TestBeU32(t *testing.T) {
	var b BeU32
	b.Set(0x12345678)
	if b[0] != 0x12 || b[1] != 0x34 || b[2] != 0x56 || b[3] != 0x78 {
		t.Errorf("Set bytes wrong: %02x %02x %02x %02x",
			b[0], b[1], b[2], b[3])
	}
	if b.Get() != 0x12345678 {
		t.Error("Get failed")
	}
}

func TestBeU32IPv4(t *testing.T) {
	// 真实 IP 地址 10.0.0.1 的大端表示应该是 0a 00 00 01
	var ip BeU32
	ip.Set(0x0A000001)
	expect := [4]byte{0x0A, 0x00, 0x00, 0x01}
	for i := 0; i < 4; i++ {
		if ip[i] != expect[i] {
			t.Errorf("IP byte %d: got %02x, want %02x", i, ip[i], expect[i])
		}
	}
}
