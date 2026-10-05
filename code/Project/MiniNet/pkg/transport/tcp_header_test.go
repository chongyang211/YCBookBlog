package transport

import (
	"bytes"
	"testing"

	netpkg "mininet/pkg/net"
)

func mustIP(t *testing.T, s string) netpkg.IPv4Addr {
	t.Helper()
	ip, err := netpkg.ParseIPv4(s)
	if err != nil {
		t.Fatal(err)
	}
	return ip
}

func TestTCPHeaderRoundtrip(t *testing.T) {
	src := mustIP(t, "10.0.0.1")
	dst := mustIP(t, "10.0.0.2")
	h := &TCPHeader{
		SrcPort: 12345,
		DstPort: 80,
		Seq:     TCPSeq(0x11223344),
		Ack:     TCPSeq(0x55667788),
		Flags:   FlagSYN | FlagACK,
		Window:  65535,
	}
	payload := []byte("hello")
	buf := h.Encode(src, dst, payload)
	if len(buf) != TCPHeaderSize+len(payload) {
		t.Fatalf("size = %d", len(buf))
	}

	h2, pl, err := DecodeTCP(src, dst, buf)
	if err != nil {
		t.Fatal(err)
	}
	if h2.SrcPort != h.SrcPort || h2.DstPort != h.DstPort {
		t.Error("port mismatch")
	}
	if h2.Seq != h.Seq || h2.Ack != h.Ack {
		t.Error("seq/ack mismatch")
	}
	if h2.Flags != h.Flags {
		t.Errorf("flags: want %s got %s", h.Flags, h2.Flags)
	}
	if !bytes.Equal(pl, payload) {
		t.Error("payload mismatch")
	}
}

func TestTCPChecksumDetectsCorruption(t *testing.T) {
	src := mustIP(t, "10.0.0.1")
	dst := mustIP(t, "10.0.0.2")
	h := &TCPHeader{SrcPort: 1, DstPort: 2, Seq: 1, Flags: FlagSYN}
	buf := h.Encode(src, dst, nil)
	buf[4] ^= 1
	_, _, err := DecodeTCP(src, dst, buf)
	if err == nil {
		t.Error("corrupted TCP should fail checksum")
	}
}

func TestFlagsString(t *testing.T) {
	cases := []struct {
		f    TCPFlags
		want string
	}{
		{0, "-"},
		{FlagSYN, "[SYN]"},
		{FlagSYN | FlagACK, "[SYN,ACK]"},
		{FlagFIN | FlagACK, "[FIN,ACK]"},
	}
	for _, c := range cases {
		if got := c.f.String(); got != c.want {
			t.Errorf("flags(%d): want %q got %q", c.f, c.want, got)
		}
	}
}

func TestStateString(t *testing.T) {
	if StateClosed.String() != "CLOSED" {
		t.Error("StateClosed name wrong")
	}
	if StateEstablished.String() != "ESTABLISHED" {
		t.Error("StateEstablished name wrong")
	}
	if StateTimeWait.String() != "TIME_WAIT" {
		t.Error("StateTimeWait name wrong")
	}
}
