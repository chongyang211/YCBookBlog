package aggregate

import (
	"testing"
	"time"
)

func TestCounter(t *testing.T) {
	c := NewCounter[string]()
	c.Inc("a")
	c.Inc("a")
	c.Add("b", 5)
	if got := c.Total(); got != 7 {
		t.Fatalf("total want 7 got %d", got)
	}
	if got := c.Len(); got != 2 {
		t.Fatalf("len want 2 got %d", got)
	}
	seen := map[string]uint64{}
	c.Range(func(k string, v uint64) bool {
		seen[k] = v
		return true
	})
	if seen["a"] != 2 || seen["b"] != 5 {
		t.Fatalf("range got %v", seen)
	}
}

func TestTopK(t *testing.T) {
	tk := NewTopK[string](3)
	for i := 0; i < 10; i++ {
		tk.Offer(string(rune('a'+i)), uint64(i)) // a=0 ... j=9
	}
	got := tk.Result()
	if len(got) != 3 {
		t.Fatalf("len want 3 got %d", len(got))
	}
	wantKeys := []string{"j", "i", "h"} // 9, 8, 7 desc
	for i, w := range wantKeys {
		if got[i].Key != w {
			t.Fatalf("pos %d want %s got %s", i, w, got[i].Key)
		}
	}
}

func TestTopK_FewerThanK(t *testing.T) {
	tk := NewTopK[int](5)
	tk.Offer(1, 10)
	tk.Offer(2, 20)
	got := tk.Result()
	if len(got) != 2 || got[0].Key != 2 || got[1].Key != 1 {
		t.Fatalf("got %+v", got)
	}
}

func TestTopK_ZeroK(t *testing.T) {
	tk := NewTopK[string](0)
	tk.Offer("a", 1)
	if got := tk.Result(); len(got) != 0 {
		t.Fatalf("want empty got %+v", got)
	}
}

func TestQPSBuckets(t *testing.T) {
	q := NewQPSBuckets(10)
	base := time.Unix(1000, 0)
	q.Add(base)
	q.Add(base.Add(5 * time.Second))  // same bucket
	q.Add(base.Add(11 * time.Second)) // next bucket
	ts, counts := q.Sorted()
	if len(ts) != 2 {
		t.Fatalf("buckets want 2 got %d", len(ts))
	}
	if counts[0] != 2 || counts[1] != 1 {
		t.Fatalf("counts got %v", counts)
	}
	if _, peak := q.Peak(); peak != 2 {
		t.Fatalf("peak want 2 got %d", peak)
	}
}
