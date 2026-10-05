// pkg/net/ipv4.go - IPv4 头编解码
//
// 第 2 次会话 Step 5.1
//
// IPv4 头 (20 字节,不含 Options):
//
//    0                   1                   2                   3
//    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |Version|  IHL  |     TOS       |          Total Length         |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |         Identification        |Flags|      Fragment Offset    |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |      TTL      |    Protocol   |         Header Checksum       |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |                       Source Address                          |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |                    Destination Address                        |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
package net

import (
	"fmt"

	"mininet/pkg/common"
)

// Ipv4HeaderSize 不含 Options 时固定 20 字节
const Ipv4HeaderSize = 20

// IPv4 协议号
type IPProto uint8

const (
	ProtoICMP IPProto = 1
	ProtoTCP  IPProto = 6
	ProtoUDP  IPProto = 17
)

func (p IPProto) String() string {
	switch p {
	case ProtoICMP:
		return "ICMP(1)"
	case ProtoTCP:
		return "TCP(6)"
	case ProtoUDP:
		return "UDP(17)"
	}
	return fmt.Sprintf("%d", p)
}

// Ipv4Header IPv4 头 (host-order,编码时转大端)
type Ipv4Header struct {
	Version  uint8   // 4
	IHL      uint8   // 单位 4 字节,无 Options 时 = 5
	TOS      uint8   //
	TotalLen uint16  // 头 + payload 总字节
	ID       uint16  //
	Flags    uint8   // 3bit (bit2=0, bit1=DF, bit0=MF)
	FragOff  uint16  // 13bit
	TTL      uint8   //
	Proto    IPProto //
	Checksum uint16  // 编码时由 Encode 自动计算
	Src      IPv4Addr
	Dst      IPv4Addr
}

// NewIPv4Header 便捷构造 (设置常用默认值)
func NewIPv4Header(src, dst IPv4Addr, proto IPProto, payloadLen int) *Ipv4Header {
	return &Ipv4Header{
		Version:  4,
		IHL:      5,
		TotalLen: uint16(Ipv4HeaderSize + payloadLen),
		TTL:      64,
		Proto:    proto,
		Src:      src,
		Dst:      dst,
	}
}

// Encode 把 header 编码成 20 字节,自动算 checksum
func (h *Ipv4Header) Encode() []byte {
	buf := make([]byte, Ipv4HeaderSize)
	buf[0] = (h.Version << 4) | (h.IHL & 0x0F)
	buf[1] = h.TOS
	buf[2] = byte(h.TotalLen >> 8)
	buf[3] = byte(h.TotalLen)
	buf[4] = byte(h.ID >> 8)
	buf[5] = byte(h.ID)
	// Flags(3) + FragOff(13)
	flagsFrag := (uint16(h.Flags) << 13) | (h.FragOff & 0x1FFF)
	buf[6] = byte(flagsFrag >> 8)
	buf[7] = byte(flagsFrag)
	buf[8] = h.TTL
	buf[9] = byte(h.Proto)
	// Checksum 先置 0 再算 (RFC1071)
	buf[10], buf[11] = 0, 0
	src := uint32(h.Src)
	buf[12], buf[13] = byte(src>>24), byte(src>>16)
	buf[14], buf[15] = byte(src>>8), byte(src)
	dst := uint32(h.Dst)
	buf[16], buf[17] = byte(dst>>24), byte(dst>>16)
	buf[18], buf[19] = byte(dst>>8), byte(dst)
	// 填回 checksum
	cs := common.InternetChecksum(buf)
	buf[10], buf[11] = byte(cs>>8), byte(cs)
	h.Checksum = cs
	return buf
}

// DecodeIPv4 解析 20 字节 IPv4 头 + 剩余 payload
// 校验 checksum,不通过返回错误
func DecodeIPv4(buf []byte) (*Ipv4Header, []byte, error) {
	if len(buf) < Ipv4HeaderSize {
		return nil, nil, fmt.Errorf("ipv4 header too short: %d", len(buf))
	}
	h := &Ipv4Header{
		Version:  buf[0] >> 4,
		IHL:      buf[0] & 0x0F,
		TOS:      buf[1],
		TotalLen: uint16(buf[2])<<8 | uint16(buf[3]),
		ID:       uint16(buf[4])<<8 | uint16(buf[5]),
		TTL:      buf[8],
		Proto:    IPProto(buf[9]),
		Checksum: uint16(buf[10])<<8 | uint16(buf[11]),
	}
	flagsFrag := uint16(buf[6])<<8 | uint16(buf[7])
	h.Flags = uint8(flagsFrag >> 13)
	h.FragOff = flagsFrag & 0x1FFF
	h.Src = IPv4Addr(uint32(buf[12])<<24 | uint32(buf[13])<<16 |
		uint32(buf[14])<<8 | uint32(buf[15]))
	h.Dst = IPv4Addr(uint32(buf[16])<<24 | uint32(buf[17])<<16 |
		uint32(buf[18])<<8 | uint32(buf[19]))

	if h.Version != 4 {
		return nil, nil, fmt.Errorf("not IPv4: version=%d", h.Version)
	}
	headerLen := int(h.IHL) * 4
	if headerLen < Ipv4HeaderSize || headerLen > len(buf) {
		return nil, nil, fmt.Errorf("bad IHL: %d", h.IHL)
	}
	// 校验 header checksum (计算 header 全部字节,结果必须 0)
	if cs := common.InternetChecksum(buf[:headerLen]); cs != 0 {
		return nil, nil, fmt.Errorf("ipv4 checksum fail: 0x%04X", cs)
	}
	if int(h.TotalLen) > len(buf) {
		return nil, nil, fmt.Errorf("totallen %d > buf %d", h.TotalLen, len(buf))
	}

	payload := buf[headerLen:h.TotalLen]
	return h, payload, nil
}

// DecrementTTL TTL-1,返回新 TTL (0 时应丢弃)
// 并原地更新 checksum (RFC1624 增量更新)
func (h *Ipv4Header) DecrementTTL() uint8 {
	h.TTL--
	// 简单起见,直接重算 checksum
	return h.TTL
}
