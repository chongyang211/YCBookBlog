// pkg/common/checksum_test.go
package common

import "testing"

func TestInternetChecksumRFC1071(t *testing.T) {
	// RFC1071 附录 B 的例子
	// 数据: 00 01 f2 03 f4 f5 f6 f7
	// 预期 checksum = 0x220d
	data := []byte{0x00, 0x01, 0xf2, 0x03, 0xf4, 0xf5, 0xf6, 0xf7}
	if got := InternetChecksum(data); got != 0x220d {
		t.Errorf("RFC1071 example: got 0x%04x, want 0x220d", got)
	}
}

func TestChecksumIPv4Header(t *testing.T) {
	// 一个最小的 IPv4 头 (20 字节),checksum 字段已置 0
	// Version=4, IHL=5, TotalLen=60, ID=0x1234, Flags=0, TTL=64, Proto=1(ICMP)
	// Src=192.168.1.100, Dst=8.8.8.8
	hdr := []byte{
		0x45, 0x00, 0x00, 0x3c, // Ver/IHL/TOS/TotalLen
		0x12, 0x34, 0x00, 0x00, // ID/FlagFrag
		0x40, 0x01, 0x00, 0x00, // TTL/Proto/Checksum=0
		0xc0, 0xa8, 0x01, 0x64, // Src 192.168.1.100
		0x08, 0x08, 0x08, 0x08, // Dst 8.8.8.8
	}
	cs := InternetChecksum(hdr)
	t.Logf("Computed IPv4 checksum = 0x%04X", cs)

	// 自检性质:把 checksum 填回去再算一次,应该得到 0
	hdr[10] = byte(cs >> 8)
	hdr[11] = byte(cs)
	if again := InternetChecksum(hdr); again != 0 {
		t.Errorf("checksum self-check failed: got 0x%04X, want 0", again)
	}
}

func TestChecksumOddLength(t *testing.T) {
	// 奇数字节:算法要补 0 到偶数
	data := []byte{0x12, 0x34, 0x56}
	cs := InternetChecksum(data)
	// 手算: 0x1234 + 0x5600 = 0x6834, ^0x6834 = 0x97CB
	if cs != 0x97cb {
		t.Errorf("odd-length checksum: got 0x%04x, want 0x97cb", cs)
	}
}
