// pkg/dns/message.go - DNS 报文编解码 (RFC1035)
//
// 第 4 次会话 Step 9.1
//
// DNS 报文结构 (UDP:53):
//
//    +---------------------+
//    |        Header       |  12 字节
//    +---------------------+
//    |       Question      |  QNAME + QTYPE + QCLASS
//    +---------------------+
//    |        Answer       |  RR (resource record)
//    +---------------------+
//    |      Authority      |  RR (NS records)
//    +---------------------+
//    |      Additional     |  RR (常是 A/AAAA for NS)
//    +---------------------+
//
// Header (12B):
//   ID(2) · Flags(2) · QDCOUNT(2) · ANCOUNT(2) · NSCOUNT(2) · ARCOUNT(2)
//
// Flags:
//   bit 0: QR (0=query, 1=response)
//   bit 1-4: OPCODE
//   bit 5: AA (Authoritative Answer)
//   bit 6: TC (Truncated → 要走 TCP)
//   bit 7: RD (Recursion Desired)
//   bit 8: RA (Recursion Available)
//   bit 11-14: RCODE
//
// QNAME 的 "标签长度前缀" 编码:
//   www.example.com → \x03www\x07example\x03com\x00
package dns

import (
	"encoding/binary"
	"errors"
	"fmt"
	"strings"
)

// Type RR 类型
type Type uint16

const (
	TypeA     Type = 1  // IPv4 地址
	TypeNS    Type = 2  // 权威域名服务器
	TypeCNAME Type = 5  // 别名
	TypeAAAA  Type = 28 // IPv6
)

func (t Type) String() string {
	switch t {
	case TypeA:
		return "A"
	case TypeNS:
		return "NS"
	case TypeCNAME:
		return "CNAME"
	case TypeAAAA:
		return "AAAA"
	}
	return fmt.Sprintf("%d", t)
}

const ClassIN = 1 // Internet

// Header 12 字节
type Header struct {
	ID      uint16
	Flags   uint16
	QDCount uint16
	ANCount uint16
	NSCount uint16
	ARCount uint16
}

// Question 一个问题
type Question struct {
	Name  string
	Type  Type
	Class uint16
}

// RR 一条资源记录
type RR struct {
	Name  string
	Type  Type
	Class uint16
	TTL   uint32
	Data  []byte // 未解析的 RDATA (调用者按 Type 自行解析)
}

// Message 完整 DNS 报文
type Message struct {
	Header      Header
	Questions   []Question
	Answers     []RR
	Authorities []RR
	Additionals []RR
}

// -------- 编码 --------

// NewQuery 构造一个标准递归查询
func NewQuery(id uint16, name string, qtype Type, recursionDesired bool) *Message {
	flags := uint16(0)
	if recursionDesired {
		flags |= 1 << 8 // RD
	}
	return &Message{
		Header: Header{ID: id, Flags: flags, QDCount: 1},
		Questions: []Question{
			{Name: name, Type: qtype, Class: ClassIN},
		},
	}
}

// Encode 把 Message 序列化成字节
func (m *Message) Encode() ([]byte, error) {
	buf := make([]byte, 12)
	binary.BigEndian.PutUint16(buf[0:], m.Header.ID)
	binary.BigEndian.PutUint16(buf[2:], m.Header.Flags)
	binary.BigEndian.PutUint16(buf[4:], uint16(len(m.Questions)))
	binary.BigEndian.PutUint16(buf[6:], uint16(len(m.Answers)))
	binary.BigEndian.PutUint16(buf[8:], uint16(len(m.Authorities)))
	binary.BigEndian.PutUint16(buf[10:], uint16(len(m.Additionals)))

	for _, q := range m.Questions {
		nb, err := encodeName(q.Name)
		if err != nil {
			return nil, err
		}
		buf = append(buf, nb...)
		var tc [4]byte
		binary.BigEndian.PutUint16(tc[0:], uint16(q.Type))
		binary.BigEndian.PutUint16(tc[2:], q.Class)
		buf = append(buf, tc[:]...)
	}
	// 简化版: 把 Answers 也编一下 (给 fake server 回包用)
	// 真实场景: Answers 的 name 通常用压缩指针 (0xC00C 指向 question),
	// 这里为了简单直接重复编码一次 name
	for _, rr := range m.Answers {
		nb, err := encodeName(rr.Name)
		if err != nil {
			return nil, err
		}
		buf = append(buf, nb...)
		var th [10]byte
		binary.BigEndian.PutUint16(th[0:], uint16(rr.Type))
		binary.BigEndian.PutUint16(th[2:], rr.Class)
		binary.BigEndian.PutUint32(th[4:], rr.TTL)
		binary.BigEndian.PutUint16(th[8:], uint16(len(rr.Data)))
		buf = append(buf, th[:]...)
		buf = append(buf, rr.Data...)
	}
	// Authorities / Additionals 本案例暂不需要序列化
	return buf, nil
}

// encodeName "www.example.com" → \x03www\x07example\x03com\x00
func encodeName(name string) ([]byte, error) {
	if len(name) > 253 {
		return nil, errors.New("name too long")
	}
	var buf []byte
	for _, label := range strings.Split(strings.TrimSuffix(name, "."), ".") {
		if len(label) == 0 || len(label) > 63 {
			return nil, fmt.Errorf("bad label %q", label)
		}
		buf = append(buf, byte(len(label)))
		buf = append(buf, []byte(label)...)
	}
	buf = append(buf, 0) // root label
	return buf, nil
}

// -------- 解码 --------

// Decode 把字节反序列化成 Message
func Decode(buf []byte) (*Message, error) {
	if len(buf) < 12 {
		return nil, errors.New("dns msg too short")
	}
	m := &Message{}
	m.Header = Header{
		ID:      binary.BigEndian.Uint16(buf[0:]),
		Flags:   binary.BigEndian.Uint16(buf[2:]),
		QDCount: binary.BigEndian.Uint16(buf[4:]),
		ANCount: binary.BigEndian.Uint16(buf[6:]),
		NSCount: binary.BigEndian.Uint16(buf[8:]),
		ARCount: binary.BigEndian.Uint16(buf[10:]),
	}

	off := 12
	// Questions
	for i := 0; i < int(m.Header.QDCount); i++ {
		name, next, err := decodeName(buf, off)
		if err != nil {
			return nil, err
		}
		if next+4 > len(buf) {
			return nil, errors.New("truncated question")
		}
		q := Question{
			Name:  name,
			Type:  Type(binary.BigEndian.Uint16(buf[next:])),
			Class: binary.BigEndian.Uint16(buf[next+2:]),
		}
		m.Questions = append(m.Questions, q)
		off = next + 4
	}

	// Answers / Authorities / Additionals
	for _, dst := range []*[]RR{&m.Answers, &m.Authorities, &m.Additionals} {
		cnt := []uint16{m.Header.ANCount, m.Header.NSCount, m.Header.ARCount}[0]
		switch dst {
		case &m.Authorities:
			cnt = m.Header.NSCount
		case &m.Additionals:
			cnt = m.Header.ARCount
		}
		for i := 0; i < int(cnt); i++ {
			rr, next, err := decodeRR(buf, off)
			if err != nil {
				return nil, err
			}
			*dst = append(*dst, rr)
			off = next
		}
	}
	return m, nil
}

// decodeRR 解析一条资源记录
func decodeRR(buf []byte, off int) (RR, int, error) {
	name, next, err := decodeName(buf, off)
	if err != nil {
		return RR{}, 0, err
	}
	if next+10 > len(buf) {
		return RR{}, 0, errors.New("truncated RR header")
	}
	rr := RR{
		Name:  name,
		Type:  Type(binary.BigEndian.Uint16(buf[next:])),
		Class: binary.BigEndian.Uint16(buf[next+2:]),
		TTL:   binary.BigEndian.Uint32(buf[next+4:]),
	}
	rdLen := int(binary.BigEndian.Uint16(buf[next+8:]))
	rdStart := next + 10
	if rdStart+rdLen > len(buf) {
		return RR{}, 0, errors.New("truncated RDATA")
	}
	rr.Data = buf[rdStart : rdStart+rdLen]
	return rr, rdStart + rdLen, nil
}

// decodeName 处理 DNS 名字 (含 compression pointer 0xC0)
// 返回: 名字字符串、下一个字段的偏移、错误
//
// 压缩指针: 字节高 2 位是 11,后 14 位是 "指向这个报文另一个位置的偏移"
// 示例: www.example.com 第一次完整写,第二次用 \xC0\x0C 指过去
func decodeName(buf []byte, off int) (string, int, error) {
	var labels []string
	jumped := false
	origOff := off
	limit := 20 // 防无限递归 (RFC 没规定上限,工程常用 20)
	for limit > 0 {
		limit--
		if off >= len(buf) {
			return "", 0, errors.New("name offset out of range")
		}
		ll := int(buf[off])
		if ll == 0 {
			off++
			if !jumped {
				origOff = off
			}
			return strings.Join(labels, "."), origOff, nil
		}
		if ll&0xC0 == 0xC0 {
			if off+1 >= len(buf) {
				return "", 0, errors.New("truncated pointer")
			}
			ptr := int(binary.BigEndian.Uint16(buf[off:])) & 0x3FFF
			if !jumped {
				origOff = off + 2
			}
			off = ptr
			jumped = true
			continue
		}
		if ll > 63 {
			return "", 0, fmt.Errorf("bad label len %d", ll)
		}
		if off+1+ll > len(buf) {
			return "", 0, errors.New("truncated label")
		}
		labels = append(labels, string(buf[off+1:off+1+ll]))
		off += 1 + ll
	}
	return "", 0, errors.New("name pointer loop")
}

// -------- Flags 辅助 --------

func (h Header) IsResponse() bool    { return h.Flags&(1<<15) != 0 }
func (h Header) Truncated() bool     { return h.Flags&(1<<9) != 0 }
func (h Header) RecursionAvail() bool { return h.Flags&(1<<7) != 0 }
func (h Header) RCode() uint8        { return uint8(h.Flags & 0x0F) }
