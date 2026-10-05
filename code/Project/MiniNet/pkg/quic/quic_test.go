package quic

import (
	"bytes"
	"sync"
	"testing"
	"time"
)

func TestPacketRoundTrip(t *testing.T) {
	orig := &Packet{
		Flags:     FlagACK,
		ConnID:    ConnID{1, 2, 3, 4, 5, 6, 7, 8},
		PacketNum: 42,
		Frames: []Frame{
			&PingFrame{},
			&StreamFrame{StreamID: 0, Offset: 0, Data: []byte("hello"), Fin: true},
			&StreamFrame{StreamID: 2, Offset: 100, Data: []byte("world"), Fin: false},
			&ACKFrame{Largest: 7},
			&PathChallengeFrame{Token: [8]byte{0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88}},
		},
	}
	wire := orig.Encode()
	got, err := Decode(wire)
	if err != nil {
		t.Fatal(err)
	}
	if got.ConnID != orig.ConnID {
		t.Error("ConnID mismatch")
	}
	if got.PacketNum != 42 {
		t.Error("PktNum mismatch")
	}
	if len(got.Frames) != 5 {
		t.Fatalf("frames = %d", len(got.Frames))
	}
	s0 := got.Frames[1].(*StreamFrame)
	if !s0.Fin || string(s0.Data) != "hello" {
		t.Errorf("stream0: fin=%v data=%q", s0.Fin, s0.Data)
	}
	s2 := got.Frames[2].(*StreamFrame)
	if s2.Offset != 100 || string(s2.Data) != "world" {
		t.Errorf("stream2: offset=%d data=%q", s2.Offset, s2.Data)
	}
}

// TestEndToEndMultiStream 核心:多流并发,证明互不阻塞
func TestEndToEndMultiStream(t *testing.T) {
	ln, err := Listen("127.0.0.1:0")
	if err != nil {
		t.Fatal(err)
	}
	defer ln.Close()

	// server: 收任何流,原样 echo
	go func() {
		conn, err := ln.Accept()
		if err != nil {
			return
		}
		for i := 0; i < 3; i++ {
			s := conn.AcceptStream(3 * time.Second)
			if s == nil {
				t.Logf("accept stream timeout")
				return
			}
			go func(st *Stream) {
				buf := make([]byte, 100)
				n, _ := st.Read(buf)
				st.WriteFin([]byte("echo:" + string(buf[:n])))
			}(s)
		}
	}()

	c, err := Dial(ln.Addr().String())
	if err != nil {
		t.Fatal(err)
	}
	defer c.Close()
	time.Sleep(20 * time.Millisecond)

	var wg sync.WaitGroup
	results := make([]string, 3)
	for i := 0; i < 3; i++ {
		wg.Add(1)
		go func(idx int) {
			defer wg.Done()
			s := c.OpenStream()
			defer s.Close()
			s.WriteFin([]byte{byte('A' + idx)})
			buf := make([]byte, 100)
			n, _ := s.Read(buf)
			results[idx] = string(buf[:n])
		}(i)
	}
	wg.Wait()
	for i, r := range results {
		want := "echo:" + string(byte('A'+i))
		if r != want {
			t.Errorf("stream %d: got %q, want %q", i, r, want)
		}
	}
}

// TestConnIDMigration 客户端切底层 UDP socket,ConnID 不变 → server 应认得
func TestConnIDMigration(t *testing.T) {
	ln, _ := Listen("127.0.0.1:0")
	defer ln.Close()

	readyCh := make(chan *Conn, 1)
	go func() {
		conn, err := ln.Accept()
		if err != nil {
			return
		}
		readyCh <- conn
		// server 持续 echo
		for {
			s := conn.AcceptStream(10 * time.Second)
			if s == nil {
				return
			}
			go func(st *Stream) {
				buf := make([]byte, 100)
				n, _ := st.Read(buf)
				st.WriteFin([]byte("srv:" + string(buf[:n])))
			}(s)
		}
	}()

	c, err := Dial(ln.Addr().String())
	if err != nil {
		t.Fatal(err)
	}
	defer c.Close()
	time.Sleep(20 * time.Millisecond)

	srvConn := <-readyCh
	originalConnID := srvConn.ConnID()
	originalPeerAddr := srvConn.PeerAddr().String()

	// 第一次通信,确认服务端认得我
	s1 := c.OpenStream()
	s1.WriteFin([]byte("before"))
	buf := make([]byte, 100)
	n, _ := s1.Read(buf)
	if string(buf[:n]) != "srv:before" {
		t.Fatalf("pre-migration got %q", buf[:n])
	}
	s1.Close()

	// 🔑 迁移: 客户端换一个底层 UDP socket (模拟 Wi-Fi 切 4G)
	if err := c.MigrateTo(); err != nil {
		t.Fatal(err)
	}
	time.Sleep(100 * time.Millisecond) // 等 server 识别

	// 关键断言: ConnID 没变 + PeerAddr 已切换
	if srvConn.ConnID() != originalConnID {
		t.Error("ConnID should NOT change after migration")
	}
	if srvConn.PeerAddr().String() == originalPeerAddr {
		t.Errorf("PeerAddr should change: still %s", srvConn.PeerAddr())
	}
	t.Logf("🎉 迁移成功: ConnID %s | PeerAddr %s → %s",
		originalConnID, originalPeerAddr, srvConn.PeerAddr())

	// 迁移后继续通信
	s2 := c.OpenStream()
	s2.WriteFin([]byte("after"))
	n, _ = s2.Read(buf)
	if string(buf[:n]) != "srv:after" {
		t.Errorf("post-migration got %q", buf[:n])
	}
	t.Logf("✅ 迁移后仍能通信: %q", buf[:n])
}

func TestDecodeTruncated(t *testing.T) {
	if _, err := Decode([]byte{1, 2, 3}); err == nil {
		t.Error("expect error for short")
	}
	// 完整头 + 坏 frame type
	buf := bytes.Repeat([]byte{0}, 13)
	buf = append(buf, 0xff) // 未知 frame type
	if _, err := Decode(buf); err == nil {
		t.Error("expect error for bad frame type")
	}
}
