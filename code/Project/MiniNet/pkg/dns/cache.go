// pkg/dns/cache.go - DNS TTL 缓存
//
// 第 4 次会话 Step 9.3
//
// 简化:
//   - 用单个 TTL 覆盖所有记录(真实 DNS 每条 RR 有自己的 TTL)
//   - 后台 GC goroutine 定期清过期
package dns

import (
	"net"
	"sync"
	"time"
)

type cacheEntry struct {
	ips       []net.IP
	expiresAt time.Time
}

type Cache struct {
	mu      sync.RWMutex
	entries map[string]cacheEntry
	ttl     time.Duration
}

func NewCache(ttl time.Duration) *Cache {
	c := &Cache{entries: make(map[string]cacheEntry), ttl: ttl}
	go c.gcLoop()
	return c
}

func (c *Cache) Get(name string) ([]net.IP, bool) {
	c.mu.RLock()
	e, ok := c.entries[name]
	c.mu.RUnlock()
	if !ok || time.Now().After(e.expiresAt) {
		return nil, false
	}
	return e.ips, true
}

func (c *Cache) Set(name string, ips []net.IP) {
	c.mu.Lock()
	c.entries[name] = cacheEntry{
		ips:       ips,
		expiresAt: time.Now().Add(c.ttl),
	}
	c.mu.Unlock()
}

func (c *Cache) gcLoop() {
	tk := time.NewTicker(30 * time.Second)
	defer tk.Stop()
	for range tk.C {
		now := time.Now()
		c.mu.Lock()
		for k, e := range c.entries {
			if now.After(e.expiresAt) {
				delete(c.entries, k)
			}
		}
		c.mu.Unlock()
	}
}

// Size 当前缓存条目数(给 dump 用)
func (c *Cache) Size() int {
	c.mu.RLock()
	defer c.mu.RUnlock()
	return len(c.entries)
}
