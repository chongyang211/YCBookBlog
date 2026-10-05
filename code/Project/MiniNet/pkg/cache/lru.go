// pkg/cache/lru.go - 带 TTL 的 LRU 缓存
//
// 第 6 次会话 Step 13.1
//
// 设计:
//   - 容量上限 N → 超过后淘汰最久未访问
//   - 每个 entry 带 ExpireAt → 过期自动失效
//   - 双向链表 (container/list) + hashmap
//   - O(1) Get / Set / Delete
package cache

import (
	"container/list"
	"sync"
	"time"
)

// Entry 缓存项
type Entry struct {
	Key      string
	Value    []byte
	ExpireAt time.Time
	// 阶段 ⑮ 预留的协商缓存字段
	ETag         string
	LastModified time.Time
}

// LRU 线程安全带 TTL 的 LRU 缓存
type LRU struct {
	cap  int
	mu   sync.Mutex
	ll   *list.List              // front = 最新
	idx  map[string]*list.Element // key → element
	hit  int64                   // 统计
	miss int64
	evict int64
	expire int64
}

func NewLRU(cap int) *LRU {
	if cap <= 0 {
		cap = 1024
	}
	return &LRU{
		cap: cap,
		ll:  list.New(),
		idx: make(map[string]*list.Element, cap),
	}
}

// Get 查缓存;hit 时返回 (entry, true),miss/expire 时返回 (nil, false)
func (c *LRU) Get(key string) (*Entry, bool) {
	c.mu.Lock()
	defer c.mu.Unlock()
	el, ok := c.idx[key]
	if !ok {
		c.miss++
		return nil, false
	}
	e := el.Value.(*Entry)
	if !e.ExpireAt.IsZero() && time.Now().After(e.ExpireAt) {
		c.ll.Remove(el)
		delete(c.idx, key)
		c.expire++
		c.miss++
		return nil, false
	}
	c.ll.MoveToFront(el) // 刚被访问 → 到头部
	c.hit++
	return e, true
}

// Set 写入 (ttl=0 表示永不过期)
func (c *LRU) Set(key string, value []byte, ttl time.Duration) {
	c.mu.Lock()
	defer c.mu.Unlock()
	var expireAt time.Time
	if ttl > 0 {
		expireAt = time.Now().Add(ttl)
	}
	if el, ok := c.idx[key]; ok {
		// 已存在 → 更新 + 置顶
		e := el.Value.(*Entry)
		e.Value = value
		e.ExpireAt = expireAt
		c.ll.MoveToFront(el)
		return
	}
	e := &Entry{Key: key, Value: value, ExpireAt: expireAt}
	el := c.ll.PushFront(e)
	c.idx[key] = el
	// 超容量 → 淘汰尾部
	for c.ll.Len() > c.cap {
		tail := c.ll.Back()
		if tail == nil {
			break
		}
		te := tail.Value.(*Entry)
		c.ll.Remove(tail)
		delete(c.idx, te.Key)
		c.evict++
	}
}

// Delete 显式删
func (c *LRU) Delete(key string) {
	c.mu.Lock()
	defer c.mu.Unlock()
	if el, ok := c.idx[key]; ok {
		c.ll.Remove(el)
		delete(c.idx, key)
	}
}

// Len 当前数量
func (c *LRU) Len() int {
	c.mu.Lock()
	defer c.mu.Unlock()
	return c.ll.Len()
}

// Stats
type Stats struct {
	Hit, Miss, Evict, Expire int64
	Len, Cap                 int
	HitRate                  float64
}

func (c *LRU) Stats() Stats {
	c.mu.Lock()
	defer c.mu.Unlock()
	total := c.hit + c.miss
	rate := 0.0
	if total > 0 {
		rate = float64(c.hit) / float64(total)
	}
	return Stats{
		Hit: c.hit, Miss: c.miss, Evict: c.evict, Expire: c.expire,
		Len: c.ll.Len(), Cap: c.cap, HitRate: rate,
	}
}

// Reset 清空 (测试用)
func (c *LRU) Reset() {
	c.mu.Lock()
	defer c.mu.Unlock()
	c.ll.Init()
	c.idx = make(map[string]*list.Element, c.cap)
	c.hit, c.miss, c.evict, c.expire = 0, 0, 0, 0
}
