// pkg/transport/tcp_header.go - TCP 头编解码
//
// 第 3 次会话 Step 6.1
//
// TCP 头 (20 字节,不含 Options):
//
//    0                   1                   2                   3
//    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |          Source Port          |       Destination Port        |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |                        Sequence Number                        |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |                    Acknowledgment Number                      |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |  Data |           |U|A|P|R|S|F|                               |
//   | Offset| Reserved  |R|C|S|S|Y|I|            Window             |
//   |       |           |G|K|H|T|N|N|                               |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |           Checksum            |         Urgent Pointer        |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Checksum 覆盖 "伪头(src/dst IP + proto + tcp_len) + TCP 头 + payload"
package transport

import (
	"fmt"

	"mininet/pkg/common"
	netpkg "mininet/pkg/net"
)

const TCPHeaderSize = 20

// TCP flags
type TCPFlags uint8

const (
	FlagFIN TCPFlags = 1 << 0
	FlagSYN TCPFlags = 1 << 1
	FlagRST TCPFlags = 1 << 2
	FlagPSH TCPFlags = 1 << 3
	FlagACK TCPFlags = 1 << 4
	FlagURG TCPFlags = 1 << 5
)

func (f TCPFlags) String() string {
	s := ""
	appendIf := func(mask TCPFlags, name string) {
		if f&mask != 0 {
			if s != "" {
				s += ","
			}
			s += name
		}
	}
	appendIf(FlagFIN, "FIN")
	appendIf(FlagSYN, "SYN")
	appendIf(FlagRST, "RST")
	appendIf(FlagPSH, "PSH")
	appendIf(FlagACK, "ACK")
	appendIf(FlagURG, "URG")
	if s == "" {
		return "-"
	}
	return "[" + s + "]"
}

// Has 便捷判断
func (f TCPFlags) Has(m TCPFlags) bool { return f&m != 0 }

// TCPHeader TCP 头 (host-order)
type TCPHeader struct {
	SrcPort uint16
	DstPort uint16
	Seq     TCPSeq
	Ack     TCPSeq
	Flags   TCPFlags
	Window  uint16
	Chksum  uint16
	Urgent  uint16
	// DataOffset 单位 4 字节,无 Options = 5
	DataOffset uint8
}

// Encode 把 TCP header + payload 编码,并自动算 checksum (含伪头)
// src/dst 用于伪头
func (h *TCPHeader) Encode(src, dst netpkg.IPv4Addr, payload []byte) []byte {
	hdrLen := TCPHeaderSize
	buf := make([]byte, hdrLen+len(payload))

	buf[0] = byte(h.SrcPort >> 8)
	buf[1] = byte(h.SrcPort)
	buf[2] = byte(h.DstPort >> 8)
	buf[3] = byte(h.DstPort)
	// seq
	sq := uint32(h.Seq)
	buf[4] = byte(sq >> 24)
	buf[5] = byte(sq >> 16)
	buf[6] = byte(sq >> 8)
	buf[7] = byte(sq)
	// ack
	ak := uint32(h.Ack)
	buf[8] = byte(ak >> 24)
	buf[9] = byte(ak >> 16)
	buf[10] = byte(ak >> 8)
	buf[11] = byte(ak)
	// data offset(高 4) + reserved(低 4,0)
	if h.DataOffset == 0 {
		h.DataOffset = 5
	}
	buf[12] = h.DataOffset << 4
	// flags
	buf[13] = byte(h.Flags)
	// window
	buf[14] = byte(h.Window >> 8)
	buf[15] = byte(h.Window)
	// checksum 先置 0
	buf[16], buf[17] = 0, 0
	// urgent
	buf[18] = byte(h.Urgent >> 8)
	buf[19] = byte(h.Urgent)

	// payload
	copy(buf[hdrLen:], payload)

	// 计算 checksum (含伪头 + tcp 头 + payload)
	cs := tcpChecksum(src, dst, buf)
	buf[16] = byte(cs >> 8)
	buf[17] = byte(cs)
	h.Chksum = cs

	return buf
}

// DecodeTCP 解析完整的 TCP 段 (头+payload) + 校验 checksum
func DecodeTCP(src, dst netpkg.IPv4Addr, buf []byte) (*TCPHeader, []byte, error) {
	if len(buf) < TCPHeaderSize {
		return nil, nil, fmt.Errorf("tcp too short: %d", len(buf))
	}
	// 校验 checksum (包含伪头 + 整个段)
	if cs := tcpChecksum(src, dst, buf); cs != 0 {
		return nil, nil, fmt.Errorf("tcp checksum fail: 0x%04X", cs)
	}
	h := &TCPHeader{
		SrcPort: uint16(buf[0])<<8 | uint16(buf[1]),
		DstPort: uint16(buf[2])<<8 | uint16(buf[3]),
		Seq: TCPSeq(uint32(buf[4])<<24 | uint32(buf[5])<<16 |
			uint32(buf[6])<<8 | uint32(buf[7])),
		Ack: TCPSeq(uint32(buf[8])<<24 | uint32(buf[9])<<16 |
			uint32(buf[10])<<8 | uint32(buf[11])),
		DataOffset: buf[12] >> 4,
		Flags:      TCPFlags(buf[13]),
		Window:     uint16(buf[14])<<8 | uint16(buf[15]),
		Chksum:     uint16(buf[16])<<8 | uint16(buf[17]),
		Urgent:     uint16(buf[18])<<8 | uint16(buf[19]),
	}
	headerLen := int(h.DataOffset) * 4
	if headerLen < TCPHeaderSize || headerLen > len(buf) {
		return nil, nil, fmt.Errorf("bad data offset: %d", h.DataOffset)
	}
	payload := buf[headerLen:]
	return h, payload, nil
}

// tcpChecksum 计算 TCP checksum (含伪头)
// 伪头格式: src(4) + dst(4) + zero(1) + proto(1) + tcp_len(2) = 12 字节
func tcpChecksum(src, dst netpkg.IPv4Addr, tcpSeg []byte) uint16 {
	pseudo := make([]byte, 12+len(tcpSeg))
	s := uint32(src)
	pseudo[0], pseudo[1] = byte(s>>24), byte(s>>16)
	pseudo[2], pseudo[3] = byte(s>>8), byte(s)
	d := uint32(dst)
	pseudo[4], pseudo[5] = byte(d>>24), byte(d>>16)
	pseudo[6], pseudo[7] = byte(d>>8), byte(d)
	pseudo[8] = 0
	pseudo[9] = byte(netpkg.ProtoTCP)
	tlen := uint16(len(tcpSeg))
	pseudo[10] = byte(tlen >> 8)
	pseudo[11] = byte(tlen)
	copy(pseudo[12:], tcpSeg)
	return common.InternetChecksum(pseudo)
}
