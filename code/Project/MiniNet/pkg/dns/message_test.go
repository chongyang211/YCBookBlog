package dns

import (
	"bytes"
	"testing"
)

func TestEncodeName(t *testing.T) {
	buf, err := encodeName("www.example.com")
	if err != nil {
		t.Fatal(err)
	}
	expected := []byte{3, 'w', 'w', 'w', 7, 'e', 'x', 'a', 'm', 'p', 'l', 'e', 3, 'c', 'o', 'm', 0}
	if !bytes.Equal(buf, expected) {
		t.Errorf("encodeName: got %v, want %v", buf, expected)
	}
}

func TestQueryEncodeDecode(t *testing.T) {
	q := NewQuery(0x1234, "example.com", TypeA, true)
	buf, err := q.Encode()
	if err != nil {
		t.Fatal(err)
	}
	// 应至少包含 header(12) + name + 4 bytes qtype/class
	if len(buf) < 12+1+7+1+3+1+4 {
		t.Errorf("too short: %d", len(buf))
	}
	// ID
	if buf[0] != 0x12 || buf[1] != 0x34 {
		t.Error("ID bytes wrong")
	}

	// Decode 回来
	m, err := Decode(buf)
	if err != nil {
		t.Fatal(err)
	}
	if m.Header.ID != 0x1234 {
		t.Error("ID roundtrip wrong")
	}
	if len(m.Questions) != 1 {
		t.Fatalf("questions = %d", len(m.Questions))
	}
	if m.Questions[0].Name != "example.com" {
		t.Errorf("name = %q", m.Questions[0].Name)
	}
	if m.Questions[0].Type != TypeA {
		t.Error("qtype wrong")
	}
}

func TestDecodeTooShort(t *testing.T) {
	if _, err := Decode([]byte{1, 2, 3}); err == nil {
		t.Error("short buffer should fail")
	}
}

func TestEncodeNameBad(t *testing.T) {
	// 标签过长
	long := make([]byte, 64)
	for i := range long {
		long[i] = 'a'
	}
	if _, err := encodeName(string(long)); err == nil {
		t.Error("64-byte label should fail")
	}
}

func TestDecodeNameCompression(t *testing.T) {
	// 手工构造一个带 compression pointer 的 buf
	// 偏移 12 开始: \x07example\x03com\x00  (14 字节, 到偏移 26)
	// 偏移 26 开始: \x03www + \xC0\x0C (指向 12,即 example.com)
	buf := make([]byte, 32)
	copy(buf[12:], []byte{7, 'e', 'x', 'a', 'm', 'p', 'l', 'e', 3, 'c', 'o', 'm', 0})
	copy(buf[26:], []byte{3, 'w', 'w', 'w', 0xC0, 0x0C})
	name, _, err := decodeName(buf, 26)
	if err != nil {
		t.Fatal(err)
	}
	if name != "www.example.com" {
		t.Errorf("name = %q, want www.example.com", name)
	}
}
