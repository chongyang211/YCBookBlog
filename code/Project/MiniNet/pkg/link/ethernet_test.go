package link

import (
	"bytes"
	"testing"
)

func TestMACParseAndString(t *testing.T) {
	cases := []struct {
		s   string
		raw [6]byte
	}{
		{"00:00:00:00:00:00", [6]byte{}},
		{"aa:bb:cc:dd:ee:01", [6]byte{0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0x01}},
		{"ff:ff:ff:ff:ff:ff", [6]byte{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}},
	}
	for _, c := range cases {
		m, err := ParseMAC(c.s)
		if err != nil {
			t.Fatalf("ParseMAC %q: %v", c.s, err)
		}
		if [6]byte(m) != c.raw {
			t.Errorf("ParseMAC %q: got %v, want %v", c.s, m, c.raw)
		}
		if m.String() != c.s {
			t.Errorf("String: got %q want %q", m.String(), c.s)
		}
	}
}

func TestMACBroadcast(t *testing.T) {
	if !Broadcast.IsBroadcast() {
		t.Error("Broadcast should be broadcast")
	}
	m, _ := ParseMAC("aa:bb:cc:dd:ee:01")
	if m.IsBroadcast() {
		t.Error("normal mac should not be broadcast")
	}
}

func TestEncodeDecodeFrame(t *testing.T) {
	dst := MustParseMAC("aa:bb:cc:dd:ee:02")
	src := MustParseMAC("aa:bb:cc:dd:ee:01")
	payload := []byte("hello world")

	frame := EncodeFrame(dst, src, EtherTypeIPv4, payload)
	if len(frame) != EtherHeaderSize+len(payload) {
		t.Fatalf("frame len = %d, want %d", len(frame), EtherHeaderSize+len(payload))
	}
	// 前 14 字节应为 dst(6) + src(6) + ethertype(2)
	if !bytes.Equal(frame[0:6], dst[:]) {
		t.Error("dst mac mismatch")
	}
	if !bytes.Equal(frame[6:12], src[:]) {
		t.Error("src mac mismatch")
	}
	if frame[12] != 0x08 || frame[13] != 0x00 {
		t.Errorf("ethertype want 0800, got %02x%02x", frame[12], frame[13])
	}

	d2, s2, et, pl, err := DecodeFrame(frame)
	if err != nil {
		t.Fatal(err)
	}
	if d2 != dst || s2 != src {
		t.Error("mac roundtrip failed")
	}
	if et != EtherTypeIPv4 {
		t.Errorf("ethertype roundtrip: %s", et)
	}
	if !bytes.Equal(pl, payload) {
		t.Error("payload roundtrip failed")
	}
}

func TestDecodeFrameTooShort(t *testing.T) {
	_, _, _, _, err := DecodeFrame([]byte{1, 2, 3})
	if err == nil {
		t.Error("expect error for short frame")
	}
}
