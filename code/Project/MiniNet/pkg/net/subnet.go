// pkg/net/subnet.go - IPv4 地址与子网
package net

import (
	"fmt"
	"strconv"
	"strings"
)

// IPv4Addr 用 uint32 存储,高位到低位依次是 a.b.c.d 的 a/b/c/d
// 例如 10.0.0.1 = 0x0A000001
//
// 为什么用 uint32 而不是 [4]byte?
//   - 子网匹配 (&) 只需 1 条指令, [4]byte 需要循环
//   - 比较 (==) 只需 1 条指令, [4]byte 需要 bytes.Equal
//   - 做 map key 时 uint32 直接哈希, [4]byte 要先转
type IPv4Addr uint32

// ParseIPv4 从 "10.0.0.1" 这样的字符串解析
func ParseIPv4(s string) (IPv4Addr, error) {
	parts := strings.Split(s, ".")
	if len(parts) != 4 {
		return 0, fmt.Errorf("invalid ipv4: %q", s)
	}
	var v uint32
	for _, p := range parts {
		n, err := strconv.Atoi(p)
		if err != nil || n < 0 || n > 255 {
			return 0, fmt.Errorf("invalid ipv4 octet %q in %q", p, s)
		}
		v = (v << 8) | uint32(n)
	}
	return IPv4Addr(v), nil
}

// String 把 IPv4Addr 变回 "10.0.0.1"
// (实现 fmt.Stringer 接口,fmt.Printf("%s", ip) 自动调)
func (ip IPv4Addr) String() string {
	v := uint32(ip)
	return fmt.Sprintf("%d.%d.%d.%d",
		(v>>24)&0xFF, (v>>16)&0xFF, (v>>8)&0xFF, v&0xFF)
}

// Subnet 表示一个 CIDR 子网,如 10.0.0.0/8
type Subnet struct {
	IP     IPv4Addr // 网络号,已经和 mask 做过与运算
	Prefix uint8    // 前缀长度 0~32
	Mask   uint32   // 由 Prefix 推出来的掩码,存起来避免重复计算
}

// ParseCIDR 解析 "10.0.0.0/8" 这样的字符串
func ParseCIDR(s string) (Subnet, error) {
	parts := strings.SplitN(s, "/", 2)
	if len(parts) != 2 {
		return Subnet{}, fmt.Errorf("invalid cidr %q", s)
	}
	ip, err := ParseIPv4(parts[0])
	if err != nil {
		return Subnet{}, err
	}
	prefix, err := strconv.Atoi(parts[1])
	if err != nil || prefix < 0 || prefix > 32 {
		return Subnet{}, fmt.Errorf("invalid prefix %q in %q", parts[1], s)
	}
	mask := prefixToMask(uint8(prefix))
	return Subnet{
		IP:     IPv4Addr(uint32(ip) & mask), // 保证网络号对齐
		Prefix: uint8(prefix),
		Mask:   mask,
	}, nil
}

// prefixToMask 把 "/8" 变成 0xFF000000
// ⚠️ 必须特判 prefix=0:
//
//	Go 规范里 uint32 << 32 是 "实现定义" 的 UB,
//	x86 上实际 wrap 回 0 位,结果等于原值而不是 0。
//	阶段 ⑤ BUG-2 的根源之一就是这个坑。
func prefixToMask(prefix uint8) uint32 {
	if prefix == 0 {
		return 0
	}
	return ^uint32(0) << (32 - prefix)
}

// Contains 判定一个 IP 是否属于本子网
func (s Subnet) Contains(ip IPv4Addr) bool {
	return (uint32(ip) & s.Mask) == uint32(s.IP)
}

// String 把 Subnet 打回 "10.0.0.0/8"
func (s Subnet) String() string {
	return fmt.Sprintf("%s/%d", s.IP, s.Prefix)
}
