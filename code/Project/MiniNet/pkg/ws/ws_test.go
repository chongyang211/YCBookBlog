package ws

import (
	"bufio"
	"bytes"
	"net"
	"sync"
	"testing"
	"time"

	mhttp "mininet/pkg/http"
)

func TestAcceptKeyRFC6455(t *testing.T) {
	// RFC6455 §1.3 样例
	key := "dGhlIHNhbXBsZSBub25jZQ=="
	want := "s3pPLMBiTxaQ9kYGzzhZRbK+xOo="
	if got := AcceptKey(key); got != want {
		t.Errorf("AcceptKey = %q, want %q", got, want)
	}
}

func TestFrameRoundTripSmall(t *testing.T) {
	for _, isClient := range []bool{false, true} {
		var buf bytes.Buffer
		orig := &Frame{Fin: true, Opcode: OpText, Payload: []byte("hello")}
		if err := WriteFrame(&buf, orig, isClient); err != nil {
			t.Fatal(err)
		}
		got, err := ReadFrame(&buf)
		if err != nil {
			t.Fatal(err)
		}
		if got.Opcode != OpText || string(got.Payload) != "hello" {
			t.Errorf("roundtrip isClient=%v failed: op=%s payload=%q", isClient, got.Opcode, got.Payload)
		}
	}
}

func TestFrameRoundTripLarge(t *testing.T) {
	// 10KB payload 走 126 (16bit) 扩展长度
	data := bytes.Repeat([]byte{'x'}, 10000)
	var buf bytes.Buffer
	WriteFrame(&buf, &Frame{Fin: true, Opcode: OpBinary, Payload: data}, true)
	got, err := ReadFrame(&buf)
	if err != nil {
		t.Fatal(err)
	}
	if !bytes.Equal(got.Payload, data) {
		t.Error("large payload mismatch")
	}
}

func TestFrameControlOversize(t *testing.T) {
	// 控制帧 > 125 应报错
	var buf bytes.Buffer
	err := WriteFrame(&buf, &Frame{Fin: true, Opcode: OpPing, Payload: bytes.Repeat([]byte{1}, 200)}, false)
	if err == nil {
		t.Error("expect error for oversized control frame")
	}
}

// TestEndToEndChat Upgrade + 文本消息 + Ping/Pong
func TestEndToEndChat(t *testing.T) {
	ln, _ := net.Listen("tcp", "127.0.0.1:0")
	defer ln.Close()
	addr := "ws://" + ln.Addr().String() + "/chat"

	// server: 收消息并原样回
	var wg sync.WaitGroup
	wg.Add(1)
	go func() {
		defer wg.Done()
		c, err := ln.Accept()
		if err != nil {
			return
		}
		br := bufio.NewReader(c)
		req, err := mhttp.ReadRequest(br)
		if err != nil {
			t.Logf("read req: %v", err)
			return
		}
		wsConn, err := Upgrade(c, br, req)
		if err != nil {
			t.Logf("upgrade: %v", err)
			return
		}
		for {
			op, payload, err := wsConn.Read()
			if err != nil {
				return
			}
			if op == OpText {
				wsConn.WriteText("echo:" + string(payload))
			}
		}
	}()

	// client
	time.Sleep(20 * time.Millisecond)
	c, err := Dial(addr)
	if err != nil {
		t.Fatal(err)
	}
	defer c.Close()
	// 发 3 条
	msgs := []string{"hello", "world", "mnet-ws"}
	for _, m := range msgs {
		if err := c.WriteText(m); err != nil {
			t.Fatal(err)
		}
		op, payload, err := c.Read()
		if err != nil {
			t.Fatal(err)
		}
		if op != OpText || string(payload) != "echo:"+m {
			t.Errorf("got %s/%q want echo:%s", op, payload, m)
		}
	}
	// 主动 ping (server 自动回 pong, 下一次 Read 内部会消化)
	if err := c.Ping([]byte("hb")); err != nil {
		t.Fatal(err)
	}
	// 再发一条文本看看能收到 (证明 pong 被 Read 吃掉了没干扰业务流)
	c.WriteText("after-ping")
	_, payload, _ := c.Read()
	if string(payload) != "echo:after-ping" {
		t.Errorf("after ping: %q", payload)
	}
}
