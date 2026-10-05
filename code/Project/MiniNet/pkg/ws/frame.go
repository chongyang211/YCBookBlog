// pkg/ws/frame.go - WebSocket 帧编解码 (RFC6455)
//
// 第 6 次会话 Step 14.1
//
// 帧格式:
//
//    0                   1                   2                   3
//    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//   +-+-+-+-+-------+-+-------------+-------------------------------+
//   |F|R|R|R| opcode|M| Payload len |    Extended payload length    |
//   |I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
//   |N|V|V|V|       |S|             |   (if payload len==126/127)   |
//   | |1|2|3|       |K|             |                               |
//   +-+-+-+-+-------+-+-------------+-+-----------------------------+
//   |                       Payload Data                            |
//
// opcode:
//   0x0 Continuation
//   0x1 Text frame
//   0x2 Binary frame
//   0x8 Close
//   0x9 Ping
//   0xA Pong
//
// 长度编码:
//   payload_len = 0~125  : 该字段就是长度
//   payload_len = 126    : 后跟 2 字节大端长度
//   payload_len = 127    : 后跟 8 字节大端长度
//
// Mask (客户端 → 服务端必 mask; 服务端 → 客户端必不 mask):
//   mask key 4 字节,payload[i] ^= key[i%4]
package ws

import (
	"encoding/binary"
	"errors"
	"fmt"
	"io"
	"math/rand"
)

// Opcode WebSocket 帧 opcode
type Opcode uint8

const (
	OpContinuation Opcode = 0x0
	OpText         Opcode = 0x1
	OpBinary       Opcode = 0x2
	OpClose        Opcode = 0x8
	OpPing         Opcode = 0x9
	OpPong         Opcode = 0xA
)

func (op Opcode) String() string {
	switch op {
	case OpContinuation:
		return "Continuation"
	case OpText:
		return "Text"
	case OpBinary:
		return "Binary"
	case OpClose:
		return "Close"
	case OpPing:
		return "Ping"
	case OpPong:
		return "Pong"
	}
	return fmt.Sprintf("Op(0x%x)", uint8(op))
}

// IsControl 是否控制帧 (Close/Ping/Pong)
// 控制帧特殊: ≤125 字节,不能分片
func (op Opcode) IsControl() bool { return op >= 0x8 }

// Frame 一个完整帧
type Frame struct {
	Fin     bool
	Opcode  Opcode
	Masked  bool
	Payload []byte
}

// MaxPayload 单帧最大 payload (教学值,RFC 允许 2^63)
const MaxPayload = 16 * 1024 * 1024 // 16MB

// ReadFrame 从 r 读一个完整帧 (阻塞)
func ReadFrame(r io.Reader) (*Frame, error) {
	var head [2]byte
	if _, err := io.ReadFull(r, head[:]); err != nil {
		return nil, err
	}
	f := &Frame{
		Fin:    head[0]&0x80 != 0,
		Opcode: Opcode(head[0] & 0x0f),
		Masked: head[1]&0x80 != 0,
	}
	plen := int(head[1] & 0x7f)

	// 扩展长度
	switch plen {
	case 126:
		var ext [2]byte
		if _, err := io.ReadFull(r, ext[:]); err != nil {
			return nil, err
		}
		plen = int(binary.BigEndian.Uint16(ext[:]))
	case 127:
		var ext [8]byte
		if _, err := io.ReadFull(r, ext[:]); err != nil {
			return nil, err
		}
		plen64 := binary.BigEndian.Uint64(ext[:])
		if plen64 > MaxPayload {
			return nil, fmt.Errorf("payload too large: %d", plen64)
		}
		plen = int(plen64)
	}

	// 控制帧长度限制
	if f.Opcode.IsControl() && plen > 125 {
		return nil, fmt.Errorf("control frame payload > 125")
	}

	// Mask key
	var maskKey [4]byte
	if f.Masked {
		if _, err := io.ReadFull(r, maskKey[:]); err != nil {
			return nil, err
		}
	}

	// Payload
	f.Payload = make([]byte, plen)
	if _, err := io.ReadFull(r, f.Payload); err != nil {
		return nil, err
	}
	// 反 mask
	if f.Masked {
		for i := 0; i < plen; i++ {
			f.Payload[i] ^= maskKey[i%4]
		}
	}
	return f, nil
}

// WriteFrame 把帧写入 w;isClient=true 时自动加 mask
func WriteFrame(w io.Writer, f *Frame, isClient bool) error {
	var head [2]byte
	if f.Fin {
		head[0] = 0x80
	}
	head[0] |= byte(f.Opcode) & 0x0f

	plen := len(f.Payload)
	if f.Opcode.IsControl() && plen > 125 {
		return errors.New("control frame payload > 125")
	}

	var extLen []byte
	switch {
	case plen <= 125:
		head[1] = byte(plen)
	case plen <= 0xffff:
		head[1] = 126
		extLen = make([]byte, 2)
		binary.BigEndian.PutUint16(extLen, uint16(plen))
	default:
		head[1] = 127
		extLen = make([]byte, 8)
		binary.BigEndian.PutUint64(extLen, uint64(plen))
	}

	var maskKey [4]byte
	if isClient {
		head[1] |= 0x80
		rand.Read(maskKey[:])
	}

	if _, err := w.Write(head[:]); err != nil {
		return err
	}
	if extLen != nil {
		if _, err := w.Write(extLen); err != nil {
			return err
		}
	}
	if isClient {
		if _, err := w.Write(maskKey[:]); err != nil {
			return err
		}
		// 发出去的 payload 要 mask
		masked := make([]byte, plen)
		for i := 0; i < plen; i++ {
			masked[i] = f.Payload[i] ^ maskKey[i%4]
		}
		if _, err := w.Write(masked); err != nil {
			return err
		}
	} else {
		if _, err := w.Write(f.Payload); err != nil {
			return err
		}
	}
	return nil
}
