package proxy_test

import (
	"net"
	"sync"
	"sync/atomic"
	"testing"
	"time"

	mhttp "mininet/pkg/http"
	"mininet/pkg/proxy"
)

// 起一个"源站",统计被调用了几次
func makeOrigin(t *testing.T, delay time.Duration) (addr string, counter *int32, stop func()) {
	t.Helper()
	cnt := int32(0)
	mux := mhttp.NewMux()
	mux.HandlePrefix("GET", "/", func(w mhttp.ResponseWriter, r *mhttp.Request) {
		atomic.AddInt32(&cnt, 1)
		if delay > 0 {
			time.Sleep(delay)
		}
		mhttp.WriteString(w, 200, "origin:"+r.URL)
	})
	ln, err := net.Listen("tcp", "127.0.0.1:0")
	if err != nil {
		t.Fatal(err)
	}
	srv := mhttp.NewServer("", mux)
	go srv.Serve(ln)
	return "http://" + ln.Addr().String(), &cnt, func() { srv.Shutdown() }
}

// TestProxyBasicPassthrough 不带缓存 → 透传
func TestProxyBasicPassthrough(t *testing.T) {
	up, cnt, stop := makeOrigin(t, 0)
	defer stop()

	p := proxy.New(up)
	mux := mhttp.NewMux()
	mux.HandlePrefix("GET", "/", p.ServeHTTP)
	ln, _ := net.Listen("tcp", "127.0.0.1:0")
	srv := mhttp.NewServer("", mux)
	go srv.Serve(ln)
	defer srv.Shutdown()
	time.Sleep(30 * time.Millisecond)

	c := &mhttp.Client{Timeout: 2 * time.Second}
	for i := 0; i < 5; i++ {
		res, err := c.Get("http://" + ln.Addr().String() + "/x")
		if err != nil {
			t.Fatal(err)
		}
		if string(res.Response.Body) != "origin:/x" {
			t.Errorf("body = %q", res.Response.Body)
		}
	}
	if atomic.LoadInt32(cnt) != 5 {
		t.Errorf("no cache → want 5 upstream, got %d", *cnt)
	}
}

// TestProxyCacheHit 带 LRU → 第 2 次起命中
func TestProxyCacheHit(t *testing.T) {
	up, cnt, stop := makeOrigin(t, 0)
	defer stop()

	p := proxy.New(up).WithCache(100)
	mux := mhttp.NewMux()
	mux.HandlePrefix("GET", "/", p.ServeHTTP)
	ln, _ := net.Listen("tcp", "127.0.0.1:0")
	srv := mhttp.NewServer("", mux)
	go srv.Serve(ln)
	defer srv.Shutdown()
	time.Sleep(30 * time.Millisecond)

	c := &mhttp.Client{Timeout: 2 * time.Second}
	addr := "http://" + ln.Addr().String() + "/foo"
	for i := 0; i < 10; i++ {
		c.Get(addr)
	}
	if atomic.LoadInt32(cnt) != 1 {
		t.Errorf("cache hit → want 1 upstream, got %d", *cnt)
	}
	t.Logf("stats: %s", p.Stats())
}

// -------- 🔥 BUG-5 现场核心 --------

// TestBug5CacheStampede 100 并发 miss → 100 次穿透 (不带 singleflight)
func TestBug5CacheStampede(t *testing.T) {
	up, cnt, stop := makeOrigin(t, 50*time.Millisecond) // 慢源 50ms
	defer stop()

	p := proxy.New(up).WithCache(100) // 只缓存, 不防击穿
	mux := mhttp.NewMux()
	mux.HandlePrefix("GET", "/", p.ServeHTTP)
	ln, _ := net.Listen("tcp", "127.0.0.1:0")
	srv := mhttp.NewServer("", mux)
	go srv.Serve(ln)
	defer srv.Shutdown()
	time.Sleep(30 * time.Millisecond)

	// 100 并发同 key, 全在首次 miss 窗口内到达
	c := &mhttp.Client{Timeout: 5 * time.Second}
	addr := "http://" + ln.Addr().String() + "/hot"
	var wg sync.WaitGroup
	for i := 0; i < 100; i++ {
		wg.Add(1)
		go func() {
			defer wg.Done()
			c.Get(addr)
		}()
	}
	wg.Wait()
	// 不带 singleflight → 50~100 次穿透都正常
	upstreamCalls := atomic.LoadInt32(cnt)
	if upstreamCalls < 30 {
		t.Errorf("stampede not reproduced: upstream=%d (expect ≥30)", upstreamCalls)
	}
	t.Logf("🔥 BUG-5 reproduced: 100 concurrent GETs → %d upstream calls (击穿!)", upstreamCalls)
}

// TestBug5CacheStampedeFixed 加 singleflight → 100 并发只 1 次穿透
func TestBug5CacheStampedeFixed(t *testing.T) {
	up, cnt, stop := makeOrigin(t, 50*time.Millisecond)
	defer stop()

	p := proxy.New(up).WithCache(100).WithGroup() // 缓存 + singleflight
	mux := mhttp.NewMux()
	mux.HandlePrefix("GET", "/", p.ServeHTTP)
	ln, _ := net.Listen("tcp", "127.0.0.1:0")
	srv := mhttp.NewServer("", mux)
	go srv.Serve(ln)
	defer srv.Shutdown()
	time.Sleep(30 * time.Millisecond)

	c := &mhttp.Client{Timeout: 5 * time.Second}
	addr := "http://" + ln.Addr().String() + "/hot"
	var wg sync.WaitGroup
	for i := 0; i < 100; i++ {
		wg.Add(1)
		go func() {
			defer wg.Done()
			c.Get(addr)
		}()
	}
	wg.Wait()
	upstreamCalls := atomic.LoadInt32(cnt)
	if upstreamCalls > 3 {
		t.Errorf("singleflight failed: upstream=%d (want ≤ 3)", upstreamCalls)
	}
	t.Logf("✅ BUG-5 fixed: 100 concurrent GETs → %d upstream calls (singleflight!)", upstreamCalls)
}
