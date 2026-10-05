// pkg/tls/hello.go - TLS ClientHello / ServerHello 字节解析器
//
// 第 5 次会话 Step 10.1-10.2
//
// 本模块只做"字节解码 + 可视化",不做密码学!
// 真正的 TLS 握手由标准库 crypto/tls 完成(见 wrap.go)。
//
// 教学目的: 让你亲眼看到 ClientHello 的字节分布,包括 SNI/ALPN/Cipher Suites.
// 工业面试里 "TLS 握手具体传了什么" 这类问题,看完就有感觉.
//
// ---------- TLS Record 层 ----------
//
// Record 结构 (5 字节头):
//   +--------+--------+--------+--------+--------+----------+
//   | Type   | Ver(2)        | Length(2)       | Payload   |
//   | 1 字节  |                                 |            |
//   +--------+--------+--------+--------+--------+----------+
//
//   Type:
//     20 = ChangeCipherSpec
//     21 = Alert
//     22 = Handshake  ← ClientHello/ServerHello 走这个
//     23 = ApplicationData
//
// ---------- Handshake 消息 ----------
//
//   +--------+------------+-------------+
//   | MsgType | Length(3) | Body        |
//   | 1 字节   | 3 字节     |             |
//   +--------+------------+-------------+
//
//   MsgType:
//     1  = ClientHello
//     2  = ServerHello
//     11 = Certificate
//     14 = ServerHelloDone
//
// ---------- ClientHello Body ----------
//
//   +------------+----------+--------+----------+------------+--------------+
//   | Version(2) | Random  | SessID | CipherS. | Compress   | Extensions   |
//   |            | 32 字节  | (1+N)  | (2+2N)   | (1+N)      | (2+N)        |
//   +------------+----------+--------+----------+------------+--------------+
//
// SNI (扩展号 0): "我要连接的域名"
// ALPN (扩展号 16): "我支持哪些应用层协议" (h2, http/1.1, h3)
package tls

import (
	"encoding/binary"
	"fmt"
	"strings"
)

// RecordType TLS record 的 ContentType
type RecordType uint8

const (
	RecTypeChangeCipherSpec RecordType = 20
	RecTypeAlert            RecordType = 21
	RecTypeHandshake        RecordType = 22
	RecTypeApplicationData  RecordType = 23
)

func (t RecordType) String() string {
	switch t {
	case RecTypeChangeCipherSpec:
		return "ChangeCipherSpec(20)"
	case RecTypeAlert:
		return "Alert(21)"
	case RecTypeHandshake:
		return "Handshake(22)"
	case RecTypeApplicationData:
		return "AppData(23)"
	}
	return fmt.Sprintf("Unknown(%d)", t)
}

// HandshakeType TLS Handshake 消息类型
type HandshakeType uint8

const (
	HSTypeClientHello     HandshakeType = 1
	HSTypeServerHello     HandshakeType = 2
	HSTypeCertificate     HandshakeType = 11
	HSTypeServerKeyExch   HandshakeType = 12
	HSTypeServerHelloDone HandshakeType = 14
	HSTypeClientKeyExch   HandshakeType = 16
	HSTypeFinished        HandshakeType = 20
)

func (t HandshakeType) String() string {
	switch t {
	case HSTypeClientHello:
		return "ClientHello"
	case HSTypeServerHello:
		return "ServerHello"
	case HSTypeCertificate:
		return "Certificate"
	case HSTypeServerKeyExch:
		return "ServerKeyExchange"
	case HSTypeServerHelloDone:
		return "ServerHelloDone"
	case HSTypeClientKeyExch:
		return "ClientKeyExchange"
	case HSTypeFinished:
		return "Finished"
	}
	return fmt.Sprintf("HSType(%d)", t)
}

// ProtoVersion TLS 版本号
type ProtoVersion uint16

const (
	VersionSSL30 ProtoVersion = 0x0300
	VersionTLS10 ProtoVersion = 0x0301
	VersionTLS11 ProtoVersion = 0x0302
	VersionTLS12 ProtoVersion = 0x0303
	VersionTLS13 ProtoVersion = 0x0304
)

func (v ProtoVersion) String() string {
	switch v {
	case VersionSSL30:
		return "SSLv3"
	case VersionTLS10:
		return "TLSv1.0"
	case VersionTLS11:
		return "TLSv1.1"
	case VersionTLS12:
		return "TLSv1.2"
	case VersionTLS13:
		return "TLSv1.3"
	}
	return fmt.Sprintf("0x%04x", uint16(v))
}

// Record TLS record 层头部 + payload
type Record struct {
	Type    RecordType
	Version ProtoVersion
	Payload []byte
}

// DecodeRecord 解析一个 Record (5 字节头 + 变长 payload)
// 返回 Record 和已消耗的字节数
func DecodeRecord(buf []byte) (*Record, int, error) {
	if len(buf) < 5 {
		return nil, 0, fmt.Errorf("record too short: %d", len(buf))
	}
	r := &Record{
		Type:    RecordType(buf[0]),
		Version: ProtoVersion(binary.BigEndian.Uint16(buf[1:3])),
	}
	plen := int(binary.BigEndian.Uint16(buf[3:5]))
	if 5+plen > len(buf) {
		return nil, 0, fmt.Errorf("record payload truncated: want %d have %d", plen, len(buf)-5)
	}
	r.Payload = buf[5 : 5+plen]
	return r, 5 + plen, nil
}

// -------- ClientHello --------

// ClientHello 解析后的 ClientHello
type ClientHello struct {
	Version       ProtoVersion // 老字段,TLS 1.3 填 TLSv1.2 伪装 (真实版本在扩展)
	Random        [32]byte
	SessionID     []byte
	CipherSuites  []uint16
	Compressions  []uint8
	Extensions    []Extension
	// 从 Extensions 里提取的常用字段 (方便展示)
	SNI               []string      // 扩展 0 server_name
	SupportedVersions []ProtoVersion // 扩展 43,TLS 1.3 真实版本
	ALPN              []string      // 扩展 16
	SupportedGroups   []uint16      // 扩展 10 (命名曲线/椭圆组)
}

// Extension TLS 扩展
type Extension struct {
	Type uint16
	Data []byte
}

// ParseClientHello 从 Handshake body (去掉 1+3 字节 MsgType+Length) 解析
func ParseClientHello(body []byte) (*ClientHello, error) {
	if len(body) < 2+32+1 {
		return nil, fmt.Errorf("ClientHello too short: %d", len(body))
	}
	ch := &ClientHello{Version: ProtoVersion(binary.BigEndian.Uint16(body[0:2]))}
	copy(ch.Random[:], body[2:34])
	off := 34

	// SessionID
	if off+1 > len(body) {
		return nil, fmt.Errorf("truncated at sessid len")
	}
	sl := int(body[off])
	off++
	if off+sl > len(body) {
		return nil, fmt.Errorf("truncated sessid")
	}
	ch.SessionID = body[off : off+sl]
	off += sl

	// CipherSuites (2 字节长度 + 2 字节每项)
	if off+2 > len(body) {
		return nil, fmt.Errorf("truncated at cipher len")
	}
	cl := int(binary.BigEndian.Uint16(body[off : off+2]))
	off += 2
	if off+cl > len(body) || cl%2 != 0 {
		return nil, fmt.Errorf("truncated/bad cipher list")
	}
	for i := 0; i < cl; i += 2 {
		ch.CipherSuites = append(ch.CipherSuites, binary.BigEndian.Uint16(body[off+i:off+i+2]))
	}
	off += cl

	// Compressions (1 字节长度 + 1 字节每项)
	if off+1 > len(body) {
		return nil, fmt.Errorf("truncated at comp len")
	}
	ml := int(body[off])
	off++
	if off+ml > len(body) {
		return nil, fmt.Errorf("truncated compressions")
	}
	ch.Compressions = body[off : off+ml]
	off += ml

	// Extensions (可能缺,SSLv3 风格)
	if off >= len(body) {
		return ch, nil
	}
	if off+2 > len(body) {
		return nil, fmt.Errorf("truncated at ext len")
	}
	el := int(binary.BigEndian.Uint16(body[off : off+2]))
	off += 2
	if off+el > len(body) {
		return nil, fmt.Errorf("truncated extensions")
	}
	extEnd := off + el
	for off < extEnd {
		if off+4 > extEnd {
			return nil, fmt.Errorf("truncated ext header")
		}
		et := binary.BigEndian.Uint16(body[off : off+2])
		dl := int(binary.BigEndian.Uint16(body[off+2 : off+4]))
		off += 4
		if off+dl > extEnd {
			return nil, fmt.Errorf("truncated ext data")
		}
		ex := Extension{Type: et, Data: body[off : off+dl]}
		ch.Extensions = append(ch.Extensions, ex)
		// 识别几个常用扩展
		switch et {
		case 0: // server_name (SNI)
			ch.SNI = parseSNIExt(ex.Data)
		case 10: // supported_groups
			ch.SupportedGroups = parseUint16List(ex.Data, 2)
		case 16: // ALPN
			ch.ALPN = parseALPNExt(ex.Data)
		case 43: // supported_versions
			ch.SupportedVersions = parseSupportedVersions(ex.Data)
		}
		off += dl
	}
	return ch, nil
}

// parseSNIExt RFC6066: 2 字节总长 + 列表项(1 类型 + 2 长度 + name)
func parseSNIExt(d []byte) []string {
	if len(d) < 2 {
		return nil
	}
	total := int(binary.BigEndian.Uint16(d[0:2]))
	if 2+total > len(d) {
		return nil
	}
	var names []string
	off := 2
	for off < 2+total {
		if off+3 > len(d) {
			break
		}
		// typ := d[off] (0 = host_name)
		nl := int(binary.BigEndian.Uint16(d[off+1 : off+3]))
		off += 3
		if off+nl > len(d) {
			break
		}
		names = append(names, string(d[off:off+nl]))
		off += nl
	}
	return names
}

// parseALPNExt RFC7301: 2 字节总长 + 项(1 字节 len + 字节串)
func parseALPNExt(d []byte) []string {
	if len(d) < 2 {
		return nil
	}
	total := int(binary.BigEndian.Uint16(d[0:2]))
	if 2+total > len(d) {
		return nil
	}
	var protos []string
	off := 2
	for off < 2+total {
		if off+1 > len(d) {
			break
		}
		pl := int(d[off])
		off++
		if off+pl > len(d) {
			break
		}
		protos = append(protos, string(d[off:off+pl]))
		off += pl
	}
	return protos
}

// parseSupportedVersions (扩展 43, ClientHello 格式): 1 字节总长 + 2 字节每项
func parseSupportedVersions(d []byte) []ProtoVersion {
	if len(d) < 1 {
		return nil
	}
	total := int(d[0])
	if 1+total > len(d) || total%2 != 0 {
		return nil
	}
	var versions []ProtoVersion
	for i := 1; i < 1+total; i += 2 {
		versions = append(versions, ProtoVersion(binary.BigEndian.Uint16(d[i:i+2])))
	}
	return versions
}

// parseUint16List "N 字节长 + 2 字节每项"
func parseUint16List(d []byte, lenSize int) []uint16 {
	if len(d) < lenSize {
		return nil
	}
	var total int
	if lenSize == 2 {
		total = int(binary.BigEndian.Uint16(d[0:2]))
	} else {
		total = int(d[0])
	}
	if lenSize+total > len(d) || total%2 != 0 {
		return nil
	}
	var out []uint16
	for i := lenSize; i < lenSize+total; i += 2 {
		out = append(out, binary.BigEndian.Uint16(d[i:i+2]))
	}
	return out
}

// -------- 可视化字符串 --------

// Describe 把 ClientHello 格式化成人类可读
func (ch *ClientHello) Describe() string {
	var b strings.Builder
	fmt.Fprintf(&b, "ClientHello:\n")
	fmt.Fprintf(&b, "  Legacy Version   : %s\n", ch.Version)
	fmt.Fprintf(&b, "  Random           : %x (32 bytes)\n", ch.Random[:8])
	fmt.Fprintf(&b, "  SessionID len    : %d\n", len(ch.SessionID))
	fmt.Fprintf(&b, "  CipherSuites     : %d items\n", len(ch.CipherSuites))
	for i, cs := range ch.CipherSuites {
		if i >= 5 {
			fmt.Fprintf(&b, "                     ... (%d more)\n", len(ch.CipherSuites)-5)
			break
		}
		fmt.Fprintf(&b, "                     - %s\n", CipherSuiteName(cs))
	}
	fmt.Fprintf(&b, "  Compressions     : %d items\n", len(ch.Compressions))
	fmt.Fprintf(&b, "  Extensions       : %d items\n", len(ch.Extensions))
	if len(ch.SNI) > 0 {
		fmt.Fprintf(&b, "    [SNI]          : %v\n", ch.SNI)
	}
	if len(ch.ALPN) > 0 {
		fmt.Fprintf(&b, "    [ALPN]         : %v\n", ch.ALPN)
	}
	if len(ch.SupportedVersions) > 0 {
		var vs []string
		for _, v := range ch.SupportedVersions {
			vs = append(vs, v.String())
		}
		fmt.Fprintf(&b, "    [SupportedVers]: %s\n", strings.Join(vs, ", "))
	}
	if len(ch.SupportedGroups) > 0 {
		fmt.Fprintf(&b, "    [Groups]       : %d items (first=%s)\n",
			len(ch.SupportedGroups), NamedGroupName(ch.SupportedGroups[0]))
	}
	return b.String()
}

// CipherSuiteName 常见套件的名字
func CipherSuiteName(cs uint16) string {
	switch cs {
	case 0x1301:
		return "TLS_AES_128_GCM_SHA256 (TLS 1.3)"
	case 0x1302:
		return "TLS_AES_256_GCM_SHA384 (TLS 1.3)"
	case 0x1303:
		return "TLS_CHACHA20_POLY1305_SHA256 (TLS 1.3)"
	case 0xc02f:
		return "TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256"
	case 0xc030:
		return "TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384"
	case 0xc02b:
		return "TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256"
	case 0xc02c:
		return "TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384"
	}
	return fmt.Sprintf("0x%04x", cs)
}

// NamedGroupName 命名曲线
func NamedGroupName(g uint16) string {
	switch g {
	case 23:
		return "secp256r1"
	case 24:
		return "secp384r1"
	case 25:
		return "secp521r1"
	case 29:
		return "x25519"
	case 30:
		return "x448"
	}
	return fmt.Sprintf("group(0x%04x)", g)
}
