package net

import (
	"bytes"
	"testing"
)

func TestICMPEchoRoundtrip(t *testing.T) {
	p := &ICMPEcho{
		Type: ICMPTypeEchoRequest,
		ID:   0x1234,
		Seq:  7,
		Data: []byte("hello world"),
	}
	buf := p.Encode()
	if buf[0] != 8 || buf[1] != 0 {
		t.Error("type/code wrong")
	}
	if buf[4] != 0x12 || buf[5] != 0x34 {
		t.Errorf("ID bytes: %02x %02x", buf[4], buf[5])
	}

	p2, err := DecodeICMPEcho(buf)
	if err != nil {
		t.Fatal(err)
	}
	if p2.Type != p.Type || p2.ID != p.ID || p2.Seq != p.Seq {
		t.Error("fields mismatch")
	}
	if !bytes.Equal(p2.Data, p.Data) {
		t.Error("data roundtrip failed")
	}
}

func TestICMPChecksumDetectsCorruption(t *testing.T) {
	p := &ICMPEcho{Type: ICMPTypeEchoRequest, ID: 1, Seq: 1}
	buf := p.Encode()
	buf[4] ^= 1 // 翻一位
	_, err := DecodeICMPEcho(buf)
	if err == nil {
		t.Error("corrupted packet should fail checksum")
	}
}
