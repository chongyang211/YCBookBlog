// pkg/common/bigendian.go - 大端类型安全封装
//
// 用法:
//
//	type Ipv4Header struct { TotalLen common.BeU16 }
//	h.TotalLen.Set(20)         // 存:自动大端
//	len := h.TotalLen.Get()    // 读:自动大端
//
// 设计意图:让"写错字节序"在编译期就不被允许。
// 网络协议 100% 大端,Go/x86 本机 100% 小端,靠类型封装把 bug 关进牢笼。
package common

// BeU16 "自知大端"的 16 位无符号整数
// 内部用 [2]byte 存,确保内存布局就是大端,直接 memcpy 到网络缓冲即可
type BeU16 [2]byte

func (b *BeU16) Set(v uint16) {
	b[0] = byte(v >> 8)
	b[1] = byte(v)
}

func (b *BeU16) Get() uint16 {
	return uint16(b[0])<<8 | uint16(b[1])
}

// BeU32 自知大端的 32 位无符号整数
type BeU32 [4]byte

func (b *BeU32) Set(v uint32) {
	b[0] = byte(v >> 24)
	b[1] = byte(v >> 16)
	b[2] = byte(v >> 8)
	b[3] = byte(v)
}

func (b *BeU32) Get() uint32 {
	return uint32(b[0])<<24 | uint32(b[1])<<16 |
		uint32(b[2])<<8 | uint32(b[3])
}
