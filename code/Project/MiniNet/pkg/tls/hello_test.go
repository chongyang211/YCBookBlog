package tls

import (
	"testing"
)

// TestDecodeRecord 用一段手工构造的 TLS record
func TestDecodeRecord(t *testing.T) {
	// 一个 Handshake record (type=22), TLS1.2 (0x0303), payload len=4
	buf := []byte{
		22, 0x03, 0x03, 0x00, 0x04, // header
		0x01, 0x02, 0x03, 0x04, // payload (假数据)
	}
	rec, consumed, err := DecodeRecord(buf)
	if err != nil {
		t.Fatal(err)
	}
	if rec.Type != RecTypeHandshake {
		t.Errorf("type = %s", rec.Type)
	}
	if rec.Version != VersionTLS12 {
		t.Errorf("ver = %s", rec.Version)
	}
	if consumed != 9 {
		t.Errorf("consumed = %d", consumed)
	}
	if len(rec.Payload) != 4 {
		t.Errorf("payload len = %d", len(rec.Payload))
	}
}

func TestDecodeRecordTruncated(t *testing.T) {
	if _, _, err := DecodeRecord([]byte{22, 3, 3, 0, 10, 1, 2}); err == nil {
		t.Error("expect error for truncated record")
	}
}

// TestCaptureAndParseClientHello 端到端:自己起个 server 拦截 crypto/tls 的 ClientHello
func TestCaptureAndParseClientHello(t *testing.T) {
	hello, raw, err := CaptureClientHello("www.example.com", []string{"h2", "http/1.1"})
	if err != nil {
		t.Fatalf("capture: %v", err)
	}
	if len(raw) < 5+4 {
		t.Fatalf("raw too short: %d", len(raw))
	}
	t.Logf("ClientHello %d bytes", len(raw))
	t.Logf("CipherSuites: %d items", len(hello.CipherSuites))
	t.Logf("Extensions:   %d items", len(hello.Extensions))

	// SNI 必须等于我们传入的 name
	if len(hello.SNI) != 1 || hello.SNI[0] != "www.example.com" {
		t.Errorf("SNI = %v, want [www.example.com]", hello.SNI)
	}
	// ALPN 应包含 "h2" "http/1.1"
	if len(hello.ALPN) != 2 || hello.ALPN[0] != "h2" || hello.ALPN[1] != "http/1.1" {
		t.Errorf("ALPN = %v", hello.ALPN)
	}
	// Go 现代 crypto/tls 会宣传 TLS1.3 (扩展 43)
	if len(hello.SupportedVersions) == 0 {
		t.Error("expect SupportedVersions extension")
	}
	// Describe 不崩就行
	s := hello.Describe()
	if len(s) < 50 {
		t.Errorf("Describe output too short: %q", s)
	}
	t.Logf("\n%s", s)
}

func TestCipherSuiteNames(t *testing.T) {
	cases := []struct {
		cs   uint16
		want string
	}{
		{0x1301, "TLS_AES_128_GCM_SHA256 (TLS 1.3)"},
		{0xc02f, "TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256"},
		{0x9999, "0x9999"},
	}
	for _, c := range cases {
		if got := CipherSuiteName(c.cs); got != c.want {
			t.Errorf("CipherSuiteName(0x%04x) = %q", c.cs, got)
		}
	}
}
