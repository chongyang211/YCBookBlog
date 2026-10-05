// pkg/quic/packet.go - MiniQUIC 包与帧编解码 (教学简化版)
//
// 第 7 次会话 Step 15.1
//
// 真·QUIC (RFC 9000) 要 2000+ 行 (TLS1.3 嵌入 + 流控 + ACK + 丢包恢复 + ...)
// 本案例教学目的: 让你看懂 QUIC 相对 TCP 的两个核心优势
//   1. 多流无队头阻塞 (一个流丢包不卡其他流)
//   2. ConnID 连接迁移 (Wi-Fi 切 4G 不断线)
//
// 真实 QUIC 对标:
//   • 我们的 ConnID         = QUIC 的 Destination Connection ID
//   • 我们的 StreamID        = QUIC 的 Stream ID
//   • 我们的 PacketType      = QUIC 的 Short Header 简化
//   • 我们的 StreamFrame     = QUIC 的 STREAM frame
//
// MiniQUIC 包格式:
//   +-----+----------+----------+--------+----------+
//   | Hdr | ConnID   | PacketNum| Frames | ...      |
//   | 1B  | 8B       | 4B       | TLV    |          |
//   +-----+----------+----------+--------+----------+
//
// Hdr 的 bit:
//   0x01 = 需要 ACK
//   0x02 = PathChallenge (用于迁移时验证新路径)
//   0x04 = PathResponse
package quic

import (
	"encoding/binary"
	"errors"
	"fmt"
)

// ConnID 连接标识 (8 字节,独立于五元组)
// 这是 QUIC 的灵魂: 客户端切换网卡,只要 ConnID 不变,服务端就认得你
type ConnID [8]byte

func (c ConnID) String() string {
	return fmt.Sprintf("%x", c[:])
}

// 包头 flag 位
const (
	FlagACK           = 0x01
	FlagPathChallenge = 0x02
	FlagPathResponse  = 0x04
)

// Packet 一个 QUIC 包 (UDP 数据报)
type Packet struct {
	Flags     uint8
	ConnID    ConnID
	PacketNum uint32
	Frames    []Frame
}

// Frame 包内一个帧 (教学简化: 只实现 Stream / Ping / PathChallenge / PathResponse / ACK)
type Frame interface {
	FrameType() FrameType
	Encode(buf []byte) []byte
}

// FrameType 帧类型
type FrameType uint8

const (
	FTStream        FrameType = 0x08 // STREAM: stream_id + offset + len + data
	FTPing          FrameType = 0x01
	FTACK           FrameType = 0x02 // ACK packet_num 列表
	FTPathChallenge FrameType = 0x1a
	FTPathResponse  FrameType = 0x1b
)

// StreamFrame 一个流的字节片段
// stream_id 的奇偶约定 (学 QUIC):
//   客户端发起: 偶数 ID (0, 2, 4, ...)
//   服务端发起: 奇数 ID (1, 3, 5, ...)
type StreamFrame struct {
	StreamID uint32
	Offset   uint64
	Data     []byte
	Fin      bool // 流结束
}

func (s *StreamFrame) FrameType() FrameType { return FTStream }

func (s *StreamFrame) Encode(buf []byte) []byte {
	buf = append(buf, byte(FTStream))
	if s.Fin {
		buf[len(buf)-1] |= 0x01
	}
	tmp := make([]byte, 4)
	binary.BigEndian.PutUint32(tmp, s.StreamID)
	buf = append(buf, tmp...)
	tmp8 := make([]byte, 8)
	binary.BigEndian.PutUint64(tmp8, s.Offset)
	buf = append(buf, tmp8...)
	binary.BigEndian.PutUint32(tmp, uint32(len(s.Data)))
	buf = append(buf, tmp...)
	buf = append(buf, s.Data...)
	return buf
}

// PingFrame 心跳 / 探测
type PingFrame struct{}

func (p *PingFrame) FrameType() FrameType       { return FTPing }
func (p *PingFrame) Encode(buf []byte) []byte   { return append(buf, byte(FTPing)) }

// PathChallengeFrame 迁移时发出 "我是不是真的在新地址"
type PathChallengeFrame struct {
	Token [8]byte
}

func (p *PathChallengeFrame) FrameType() FrameType { return FTPathChallenge }
func (p *PathChallengeFrame) Encode(buf []byte) []byte {
	buf = append(buf, byte(FTPathChallenge))
	return append(buf, p.Token[:]...)
}

// PathResponseFrame 对方回 token 证明路径
type PathResponseFrame struct {
	Token [8]byte
}

func (p *PathResponseFrame) FrameType() FrameType { return FTPathResponse }
func (p *PathResponseFrame) Encode(buf []byte) []byte {
	buf = append(buf, byte(FTPathResponse))
	return append(buf, p.Token[:]...)
}

// ACKFrame 教学简化: 只传一个最大已收到的 packet number
type ACKFrame struct {
	Largest uint32
}

func (a *ACKFrame) FrameType() FrameType { return FTACK }
func (a *ACKFrame) Encode(buf []byte) []byte {
	buf = append(buf, byte(FTACK))
	tmp := make([]byte, 4)
	binary.BigEndian.PutUint32(tmp, a.Largest)
	return append(buf, tmp...)
}

// -------- Packet 编解码 --------

// Encode Packet → wire bytes
func (p *Packet) Encode() []byte {
	buf := make([]byte, 0, 128)
	buf = append(buf, p.Flags)
	buf = append(buf, p.ConnID[:]...)
	tmp := make([]byte, 4)
	binary.BigEndian.PutUint32(tmp, p.PacketNum)
	buf = append(buf, tmp...)
	for _, f := range p.Frames {
		buf = f.Encode(buf)
	}
	return buf
}

// Decode wire → Packet
func Decode(buf []byte) (*Packet, error) {
	if len(buf) < 1+8+4 {
		return nil, errors.New("packet too short")
	}
	p := &Packet{Flags: buf[0]}
	copy(p.ConnID[:], buf[1:9])
	p.PacketNum = binary.BigEndian.Uint32(buf[9:13])
	off := 13
	for off < len(buf) {
		ftRaw := buf[off]
		ft := FrameType(ftRaw & 0xf8) // STREAM 占高 5 位 + 低 3 位 flag;其他类型精确匹配
		fin := false
		if ftRaw&^0x01 == byte(FTStream) {
			ft = FTStream
			fin = ftRaw&0x01 != 0
		} else {
			ft = FrameType(ftRaw)
		}
		off++
		switch ft {
		case FTStream:
			if off+16 > len(buf) {
				return nil, errors.New("truncated STREAM frame")
			}
			sid := binary.BigEndian.Uint32(buf[off : off+4])
			offset := binary.BigEndian.Uint64(buf[off+4 : off+12])
			dl := int(binary.BigEndian.Uint32(buf[off+12 : off+16]))
			off += 16
			if off+dl > len(buf) {
				return nil, errors.New("truncated STREAM data")
			}
			p.Frames = append(p.Frames, &StreamFrame{
				StreamID: sid,
				Offset:   offset,
				Data:     append([]byte{}, buf[off:off+dl]...),
				Fin:      fin,
			})
			off += dl
		case FTPing:
			p.Frames = append(p.Frames, &PingFrame{})
		case FTACK:
			if off+4 > len(buf) {
				return nil, errors.New("truncated ACK")
			}
			p.Frames = append(p.Frames, &ACKFrame{
				Largest: binary.BigEndian.Uint32(buf[off : off+4]),
			})
			off += 4
		case FTPathChallenge:
			if off+8 > len(buf) {
				return nil, errors.New("truncated PATH_CHALLENGE")
			}
			var t [8]byte
			copy(t[:], buf[off:off+8])
			p.Frames = append(p.Frames, &PathChallengeFrame{Token: t})
			off += 8
		case FTPathResponse:
			if off+8 > len(buf) {
				return nil, errors.New("truncated PATH_RESPONSE")
			}
			var t [8]byte
			copy(t[:], buf[off:off+8])
			p.Frames = append(p.Frames, &PathResponseFrame{Token: t})
			off += 8
		default:
			return nil, fmt.Errorf("unknown frame type 0x%02x", ftRaw)
		}
	}
	return p, nil
}
