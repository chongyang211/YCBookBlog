// pkg/cache/singleflight.go - singleflight 模式 (与 pkg/dns 的类似)
//
// 第 6 次会话 Step 13.3
//
// 问题: 1000 并发同 key 缓存 miss → 1000 次穿透到源站 (击穿)
// 解法: N 并发合并成 1 次真实调用, 其余等结果
//
// 跟 pkg/dns 的 singleflight 几乎同构,但独立拷贝一份是为了:
//   1. cache 包不依赖 dns
//   2. 教学明确: 这个模式是通用的,不绑定 DNS
package cache

import "sync"

type call struct {
	wg  sync.WaitGroup
	val []byte
	err error
}

// Group 合并对同 key 的并发调用
type Group struct {
	mu sync.Mutex
	m  map[string]*call
}

func NewGroup() *Group {
	return &Group{m: make(map[string]*call)}
}

// Do 合并执行;同 key 并发只会调一次 fn
// 返回: value, err, shared (是否复用了别人的结果)
func (g *Group) Do(key string, fn func() ([]byte, error)) ([]byte, error, bool) {
	g.mu.Lock()
	if c, ok := g.m[key]; ok {
		g.mu.Unlock()
		c.wg.Wait()
		return c.val, c.err, true
	}
	c := &call{}
	c.wg.Add(1)
	g.m[key] = c
	g.mu.Unlock()

	c.val, c.err = fn()
	c.wg.Done()

	g.mu.Lock()
	delete(g.m, key)
	g.mu.Unlock()

	return c.val, c.err, false
}
