package link

import (
	"bytes"
	"testing"
	"time"
)

func TestLoopbackPair(t *testing.T) {
	a, b := LoopbackPair(
		MustParseMAC("aa:bb:cc:dd:ee:01"),
		MustParseMAC("aa:bb:cc:dd:ee:02"),
	)
	defer a.Close()
	defer b.Close()

	msg := []byte("ping!")
	if err := a.Send(msg); err != nil {
		t.Fatal(err)
	}
	got, err := b.Recv()
	if err != nil {
		t.Fatal(err)
	}
	if !bytes.Equal(got, msg) {
		t.Errorf("want %q got %q", msg, got)
	}

	// 反向也要通
	if err := b.Send([]byte("pong!")); err != nil {
		t.Fatal(err)
	}
	got, _ = a.Recv()
	if !bytes.Equal(got, []byte("pong!")) {
		t.Error("reverse direction broken")
	}
}

func TestLoopbackCloseUnblocks(t *testing.T) {
	a, b := LoopbackPair(MAC{}, MAC{})
	defer b.Close()

	done := make(chan struct{})
	go func() {
		a.Recv() // 应被 Close 立刻唤醒
		close(done)
	}()
	time.Sleep(20 * time.Millisecond)
	a.Close()

	select {
	case <-done:
	case <-time.After(500 * time.Millisecond):
		t.Fatal("Close did not unblock Recv")
	}
}
