// pkg/net/route.go - 路由表 + 最长前缀匹配
package net

import (
	"fmt"
	"sort"
	"strings"
	"sync"
)

type Route struct {
	Dst     Subnet   // 目的子网
	NextHop IPv4Addr // 下一跳
	Iface   string   // 出口接口名,阶段 ⑤ 用
}

// RouteTable 全栈路由表 (暂时只用全局单例)
//
// 核心策略: 最长前缀匹配。
// 实现: Add 时按 prefix 降序排,Lookup 时第一个 Contains 命中即最优。
type RouteTable struct {
	mu     sync.RWMutex
	routes []Route
}

func NewRouteTable() *RouteTable { return &RouteTable{} }

func (t *RouteTable) Add(r Route) {
	t.mu.Lock()
	defer t.mu.Unlock()
	t.routes = append(t.routes, r)
	// ⚠️ 必须 SliceStable: 相同 prefix 的路由保留 Add 顺序
	sort.SliceStable(t.routes, func(i, j int) bool {
		return t.routes[i].Dst.Prefix > t.routes[j].Dst.Prefix
	})
}

// Lookup 最长前缀匹配,返回命中路由(ok=true)或空(ok=false)
func (t *RouteTable) Lookup(ip IPv4Addr) (Route, bool) {
	t.mu.RLock()
	defer t.mu.RUnlock()
	for _, r := range t.routes {
		if r.Dst.Contains(ip) {
			return r, true
		}
	}
	return Route{}, false
}

// Show 把路由表打印成表格,给 REPL 的 `route show` 用
// 格式刻意对齐 Linux `ip route show` 风格,阶段 ⑤ 调试时可以并排看
func (t *RouteTable) Show() string {
	t.mu.RLock()
	defer t.mu.RUnlock()
	if len(t.routes) == 0 {
		return "(empty route table)\n"
	}
	var b strings.Builder
	b.WriteString("DESTINATION          NEXT-HOP         IFACE\n")
	for _, r := range t.routes {
		iface := r.Iface
		if iface == "" {
			iface = "-"
		}
		fmt.Fprintf(&b, "%-20s %-16s %s\n", r.Dst, r.NextHop, iface)
	}
	return b.String()
}
