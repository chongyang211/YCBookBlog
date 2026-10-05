package net

import (
	"bytes"
	"testing"
)

func TestIPv4HeaderRoundtrip(t *testing.T) {
	src, _ := ParseIPv4("10.0.0.1")
	dst, _ := ParseIPv4("10.0.0.2")
	h := NewIPv4Header(src, dst, ProtoICMP, 10)
	h.ID = 0x1234

	buf := h.Encode()
	if len(buf) != Ipv4HeaderSize {
		t.Fatalf("header size = %d, want 20", len(buf))
	}

	// 必须是大端 (TotalLen=30 → 00 1E)
	if buf[2] != 0x00 || buf[3] != 0x1E {
		t.Errorf("totallen bytes: %02x %02x, want 00 1E", buf[2], buf[3])
	}

	// 补齐 payload 后 decode
	full := append(buf, make([]byte, 10)...)
	h2, pl, err := DecodeIPv4(full)
	if err != nil {
		t.Fatal(err)
	}
	if h2.Src != src || h2.Dst != dst {
		t.Error("addr mismatch")
	}
	if h2.Proto != ProtoICMP {
		t.Error("proto mismatch")
	}
	if h2.TotalLen != 30 {
		t.Error("totallen mismatch")
	}
	if len(pl) != 10 {
		t.Errorf("payload len = %d", len(pl))
	}
}

func TestIPv4ChecksumDetectsCorruption(t *testing.T) {
	src, _ := ParseIPv4("10.0.0.1")
	dst, _ := ParseIPv4("10.0.0.2")
	h := NewIPv4Header(src, dst, ProtoICMP, 0)
	buf := h.Encode()
	// 翻转一位 bit → checksum 应该失败
	buf[12] ^= 0x01
	_, _, err := DecodeIPv4(buf)
	if err == nil {
		t.Error("corrupted header should fail checksum")
	}
}

func TestIPv4DecodeTooShort(t *testing.T) {
	_, _, err := DecodeIPv4([]byte{0x45, 0, 0, 10})
	if err == nil {
		t.Error("short buffer should fail")
	}
}

func TestIPv4DecodeWrongVersion(t *testing.T) {
	buf := make([]byte, Ipv4HeaderSize)
	buf[0] = 0x65 // Version=6
	_, _, err := DecodeIPv4(buf)
	if err == nil {
		t.Error("v6 should fail v4 decoder")
	}
}

func TestIPv4DecrementTTL(t *testing.T) {
	src, _ := ParseIPv4("10.0.0.1")
	dst, _ := ParseIPv4("10.0.0.2")
	h := NewIPv4Header(src, dst, ProtoICMP, 0)
	initial := h.TTL
	newTTL := h.DecrementTTL()
	if newTTL != initial-1 {
		t.Errorf("TTL = %d, want %d", newTTL, initial-1)
	}
}

func TestIPv4PayloadBoundary(t *testing.T) {
	// 构造一个 TotalLen=25 的头,但给 30 字节 buf → 只应读 5 字节 payload
	src, _ := ParseIPv4("1.2.3.4")
	dst, _ := ParseIPv4("5.6.7.8")
	h := NewIPv4Header(src, dst, ProtoICMP, 5)
	buf := h.Encode()
	full := append(buf, []byte{1, 2, 3, 4, 5, 0xFF, 0xFF}...)
	_, pl, err := DecodeIPv4(full)
	if err != nil {
		t.Fatal(err)
	}
	if !bytes.Equal(pl, []byte{1, 2, 3, 4, 5}) {
		t.Errorf("payload boundary wrong: %v", pl)
	}
}
