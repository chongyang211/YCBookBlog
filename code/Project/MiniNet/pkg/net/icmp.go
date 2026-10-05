// pkg/net/icmp.go - ICMP Echo (ping) 协议
//
// 第 2 次会话 Step 5.2
//
// ICMP 头 (8 字节,仅 Echo request/reply):
//
//    0                   1                   2                   3
//    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |     Type      |     Code      |          Checksum             |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |         Identifier            |        Sequence Number        |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |                             Data...                           |
//
// Type: 8 = Echo request, 0 = Echo reply
// Code: 一律 0
// Checksum: 覆盖整个 ICMP (头 + data),计算时 Checksum 字段先置 0
package net

import (
	"fmt"

	"mininet/pkg/common"
)

type ICMPType uint8

const (
	ICMPTypeEchoReply   ICMPType = 0
	ICMPTypeEchoRequest ICMPType = 8
)

const ICMPHeaderSize = 8

type ICMPEcho struct {
	Type     ICMPType
	Code     uint8
	Checksum uint16
	ID       uint16
	Seq      uint16
	Data     []byte // 可选 payload (ping 常放时间戳用于 RTT)
}

// Encode 编码成完整的 ICMP 字节 (头 + data),自动算 checksum
func (p *ICMPEcho) Encode() []byte {
	buf := make([]byte, ICMPHeaderSize+len(p.Data))
	buf[0] = byte(p.Type)
	buf[1] = p.Code
	// Checksum 先置 0
	buf[2], buf[3] = 0, 0
	buf[4] = byte(p.ID >> 8)
	buf[5] = byte(p.ID)
	buf[6] = byte(p.Seq >> 8)
	buf[7] = byte(p.Seq)
	copy(buf[8:], p.Data)
	cs := common.InternetChecksum(buf)
	buf[2], buf[3] = byte(cs>>8), byte(cs)
	p.Checksum = cs
	return buf
}

// DecodeICMPEcho 解析 ICMP Echo (校验 checksum)
func DecodeICMPEcho(buf []byte) (*ICMPEcho, error) {
	if len(buf) < ICMPHeaderSize {
		return nil, fmt.Errorf("icmp too short: %d", len(buf))
	}
	if cs := common.InternetChecksum(buf); cs != 0 {
		return nil, fmt.Errorf("icmp checksum fail: 0x%04X", cs)
	}
	p := &ICMPEcho{
		Type:     ICMPType(buf[0]),
		Code:     buf[1],
		Checksum: uint16(buf[2])<<8 | uint16(buf[3]),
		ID:       uint16(buf[4])<<8 | uint16(buf[5]),
		Seq:      uint16(buf[6])<<8 | uint16(buf[7]),
	}
	if len(buf) > ICMPHeaderSize {
		p.Data = append([]byte{}, buf[ICMPHeaderSize:]...)
	}
	return p, nil
}
