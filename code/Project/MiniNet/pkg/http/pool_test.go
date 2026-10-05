package http_test

import (
	"net"
	"testing"
	"time"

	mhttp "mininet/pkg/http"
)

// TestConnPoolReuse 池命中 → isReused=true
func TestConnPoolReuse(t *testing.T) {
	// 用 pipe 做假 net.Conn
	ln, _ := net.Listen("tcp", "127.0.0.1:0")
	defer ln.Close()
	go func() {
		for {
			c, err := ln.Accept()
			if err != nil {
				return
			}
			go func(c net.Conn) {
				buf := make([]byte, 1024)
				for {
					c.SetReadDeadline(time.Now().Add(2 * time.Second))
					if _, err := c.Read(buf); err != nil {
						return
					}
				}
			}(c)
		}
	}()

	pool := mhttp.NewConnPool(2, 10*time.Second)
	dial := func(host string) (net.Conn, error) {
		return net.Dial("tcp", host)
	}
	host := ln.Addr().String()

	// 第 1 次拿 → 新建
	c1, reused1, err := pool.Get(host, dial)
	if err != nil {
		t.Fatal(err)
	}
	if reused1 {
		t.Error("first get should be new")
	}
	pool.Put(host, c1, false) // 塞回去

	// 第 2 次拿 → 复用
	c2, reused2, err := pool.Get(host, dial)
	if err != nil {
		t.Fatal(err)
	}
	if !reused2 {
		t.Error("second get should reuse")
	}
	if c2 != c1 {
		t.Error("should get same conn back")
	}
	pool.Put(host, c2, true) // close
	pool.Close()
}

// TestConnPoolMaxPer 超过 maxPer 就关
func TestConnPoolMaxPer(t *testing.T) {
	ln, _ := net.Listen("tcp", "127.0.0.1:0")
	defer ln.Close()
	go func() {
		for {
			c, err := ln.Accept()
			if err != nil {
				return
			}
			defer c.Close()
			_ = c
		}
	}()
	pool := mhttp.NewConnPool(1, 10*time.Second) // 只缓存 1 个
	dial := func(h string) (net.Conn, error) { return net.Dial("tcp", h) }
	host := ln.Addr().String()

	c1, _, _ := pool.Get(host, dial)
	c2, _, _ := pool.Get(host, dial)
	pool.Put(host, c1, false) // 塞进去 (池 1/1)
	pool.Put(host, c2, false) // 塞不进 → 关

	stats := pool.Stats()
	if stats[host] != 1 {
		t.Errorf("expect pool=1, got %d", stats[host])
	}
	pool.Close()
}

// TestConnPoolExpire 过期连接 Get 时自动丢弃
func TestConnPoolExpire(t *testing.T) {
	ln, _ := net.Listen("tcp", "127.0.0.1:0")
	defer ln.Close()
	go func() {
		for {
			if c, err := ln.Accept(); err == nil {
				defer c.Close()
			} else {
				return
			}
		}
	}()
	pool := mhttp.NewConnPool(2, 50*time.Millisecond) // 50ms 过期
	dial := func(h string) (net.Conn, error) { return net.Dial("tcp", h) }
	host := ln.Addr().String()

	c1, _, _ := pool.Get(host, dial)
	pool.Put(host, c1, false)
	time.Sleep(100 * time.Millisecond) // 等过期

	_, reused, _ := pool.Get(host, dial)
	if reused {
		t.Error("expired conn should NOT be reused")
	}
	pool.Close()
}
