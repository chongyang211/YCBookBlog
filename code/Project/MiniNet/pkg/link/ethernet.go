// pkg/link/ethernet.go - MAC 地址 + 以太帧编解码
//
// 第 2 次会话 Step 4.2
//
// 以太网帧结构 (DIX 版本, 无 802.1Q VLAN):
//
//   +--------+--------+--------+----------------+
//   | DstMAC | SrcMAC | EType  |     Payload    |
//   | 6 字节  | 6 字节  | 2 字节 |   46~1500 字节  |
//   +--------+--------+--------+----------------+
//
// EtherType 常见值:
//   0x0800 = IPv4
//   0x0806 = ARP
//   0x86DD = IPv6
package link

import (
	"encoding/hex"
	"fmt"
	"strings"
)

// MAC 以太网硬件地址
type MAC [6]byte

// ParseMAC 从 "aa:bb:cc:dd:ee:01" 这样的字符串解析
func ParseMAC(s string) (MAC, error) {
	var m MAC
	parts := strings.Split(s, ":")
	if len(parts) != 6 {
		return m, fmt.Errorf("invalid mac: %q", s)
	}
	for i, p := range parts {
		b, err := hex.DecodeString(p)
		if err != nil || len(b) != 1 {
			return m, fmt.Errorf("invalid mac octet %q in %q", p, s)
		}
		m[i] = b[0]
	}
	return m, nil
}

// MustParseMAC 内部用,解析失败 panic (测试/初始化用)
func MustParseMAC(s string) MAC {
	m, err := ParseMAC(s)
	if err != nil {
		panic(err)
	}
	return m
}

func (m MAC) String() string {
	return fmt.Sprintf("%02x:%02x:%02x:%02x:%02x:%02x",
		m[0], m[1], m[2], m[3], m[4], m[5])
}

// IsBroadcast ff:ff:ff:ff:ff:ff
func (m MAC) IsBroadcast() bool {
	for _, b := range m {
		if b != 0xFF {
			return false
		}
	}
	return true
}

// Broadcast 广播 MAC
var Broadcast = MAC{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}

// EtherType 以太帧 Type 字段
type EtherType uint16

const (
	EtherTypeIPv4 EtherType = 0x0800
	EtherTypeARP  EtherType = 0x0806
	EtherTypeIPv6 EtherType = 0x86DD
)

func (et EtherType) String() string {
	switch et {
	case EtherTypeIPv4:
		return "IPv4(0x0800)"
	case EtherTypeARP:
		return "ARP(0x0806)"
	case EtherTypeIPv6:
		return "IPv6(0x86DD)"
	}
	return fmt.Sprintf("0x%04X", uint16(et))
}

// EtherHeaderSize 以太帧头固定 14 字节
const EtherHeaderSize = 14

// EncodeFrame 把头 + payload 打包成一帧字节
// 不做 FCS (以太网校验由硬件做,软件层统一省略)
func EncodeFrame(dst, src MAC, et EtherType, payload []byte) []byte {
	buf := make([]byte, EtherHeaderSize+len(payload))
	copy(buf[0:6], dst[:])
	copy(buf[6:12], src[:])
	buf[12] = byte(et >> 8)
	buf[13] = byte(et)
	copy(buf[14:], payload)
	return buf
}

// DecodeFrame 把一帧字节解包成头字段 + payload (payload 是子切片,零拷贝)
func DecodeFrame(buf []byte) (dst, src MAC, et EtherType, payload []byte, err error) {
	if len(buf) < EtherHeaderSize {
		err = fmt.Errorf("frame too short: %d bytes", len(buf))
		return
	}
	copy(dst[:], buf[0:6])
	copy(src[:], buf[6:12])
	et = EtherType(uint16(buf[12])<<8 | uint16(buf[13]))
	payload = buf[14:]
	return
}
