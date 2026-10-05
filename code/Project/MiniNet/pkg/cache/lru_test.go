package cache

import (
	"fmt"
	"sync"
	"sync/atomic"
	"testing"
	"time"
)

func TestLRUBasic(t *testing.T) {
	c := NewLRU(3)
	c.Set("a", []byte("1"), 0)
	c.Set("b", []byte("2"), 0)
	c.Set("c", []byte("3"), 0)
	if e, ok := c.Get("a"); !ok || string(e.Value) != "1" {
		t.Error("a miss")
	}
	// a 被访问后 LRU 顺序: a c b (a 最新)
	c.Set("d", []byte("4"), 0) // 淘汰 b (最老)
	if _, ok := c.Get("b"); ok {
		t.Error("b should be evicted")
	}
	if _, ok := c.Get("a"); !ok {
		t.Error("a should still exist")
	}
	s := c.Stats()
	if s.Evict != 1 {
		t.Errorf("evict = %d", s.Evict)
	}
}

func TestLRUTTL(t *testing.T) {
	c := NewLRU(10)
	c.Set("x", []byte("y"), 50*time.Millisecond)
	if _, ok := c.Get("x"); !ok {
		t.Error("x should hit")
	}
	time.Sleep(100 * time.Millisecond)
	if _, ok := c.Get("x"); ok {
		t.Error("x should expire")
	}
	s := c.Stats()
	if s.Expire != 1 {
		t.Errorf("expire = %d", s.Expire)
	}
}

func TestLRUUpdate(t *testing.T) {
	c := NewLRU(10)
	c.Set("k", []byte("v1"), 0)
	c.Set("k", []byte("v2"), 0) // 覆盖
	if e, _ := c.Get("k"); string(e.Value) != "v2" {
		t.Error("update failed")
	}
	if c.Len() != 1 {
		t.Error("duplicate entry")
	}
}

// TestSingleflight 10 并发同 key → 1 次 fn 调用
func TestSingleflight(t *testing.T) {
	g := NewGroup()
	var called int32
	var wg sync.WaitGroup
	sharedCount := int32(0)
	for i := 0; i < 10; i++ {
		wg.Add(1)
		go func() {
			defer wg.Done()
			_, _, shared := g.Do("k", func() ([]byte, error) {
				atomic.AddInt32(&called, 1)
				time.Sleep(50 * time.Millisecond) // 让其他 goroutine 都进来等
				return []byte("ok"), nil
			})
			if shared {
				atomic.AddInt32(&sharedCount, 1)
			}
		}()
	}
	wg.Wait()
	if called != 1 {
		t.Errorf("fn called %d times, want 1", called)
	}
	if sharedCount < 8 {
		t.Errorf("only %d shared, want ≥ 8", sharedCount)
	}
	t.Logf("called=%d shared=%d", called, sharedCount)
}

// BenchmarkLRU 看看吞吐
func BenchmarkLRU(b *testing.B) {
	c := NewLRU(1000)
	for i := 0; i < 1000; i++ {
		c.Set(fmt.Sprintf("k%d", i), []byte("v"), 0)
	}
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		c.Get(fmt.Sprintf("k%d", i%1000))
	}
}
