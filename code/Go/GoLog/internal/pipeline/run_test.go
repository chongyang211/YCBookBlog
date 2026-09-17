package pipeline

import (
	"context"
	"strings"
	"testing"
)

func TestRun_Streaming(t *testing.T) {
	log := strings.Repeat(
		`1.2.3.4 - - [23/May/2026:20:30:12 +0800] "GET /a HTTP/1.1" 200 1 "-" "-"`+"\n",
		10000)
	s := NewStats(1)
	if err := Run(context.Background(), strings.NewReader(log), s); err != nil {
		t.Fatal(err)
	}
	if s.TotalLines != 10000 || s.BadLines != 0 {
		t.Fatalf("total=%d bad=%d", s.TotalLines, s.BadLines)
	}
	if s.IPCount.Total() != 10000 {
		t.Fatalf("ip total = %d", s.IPCount.Total())
	}
	if s.StatusBuckets[1] != 10000 { // 2xx
		t.Fatalf("2xx = %d", s.StatusBuckets[1])
	}
}

func TestRun_BadLinesCounted(t *testing.T) {
	in := "garbage\n1.2.3.4 - - [23/May/2026:20:30:12 +0800] \"GET /a HTTP/1.1\" 200 1 \"-\" \"-\"\nalso garbage\n"
	s := NewStats(1)
	if err := Run(context.Background(), strings.NewReader(in), s); err != nil {
		t.Fatal(err)
	}
	if s.TotalLines != 3 || s.BadLines != 2 {
		t.Fatalf("total=%d bad=%d", s.TotalLines, s.BadLines)
	}
}

func TestRun_ContextCancel(t *testing.T) {
	log := strings.Repeat(
		`1.2.3.4 - - [23/May/2026:20:30:12 +0800] "GET /a HTTP/1.1" 200 1 "-" "-"`+"\n",
		100)
	ctx, cancel := context.WithCancel(context.Background())
	cancel() // 提前取消，首行即中止
	s := NewStats(1)
	if err := Run(ctx, strings.NewReader(log), s); err != context.Canceled {
		t.Fatalf("want context.Canceled got %v", err)
	}
}
