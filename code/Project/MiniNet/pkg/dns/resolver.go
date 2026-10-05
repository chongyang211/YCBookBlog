// pkg/dns/resolver.go - DNS 递归解析器 + TTL 缓存 + singleflight
//
// 第 4 次会话 Step 9.2-9.3
//
// 递归解析流程 (以 www.example.com 为例):
//
//   1. 问 a.root-servers.net (198.41.0.4) : "www.example.com A?"
//      → 返回 Authority: com NS = a.gtld-servers.net
//           Additional: a.gtld-servers.net A = 192.5.6.30
//
//   2. 问 192.5.6.30 : "www.example.com A?"
//      → 返回 Authority: example.com NS = a.iana-servers.net
//           Additional: a.iana-servers.net A = 199.43.135.53
//
//   3. 问 199.43.135.53 : "www.example.com A?"
//      → 返回 Answer: www.example.com A = 93.184.216.34  ✅
//
// 本实现用真实 OS UDP socket (不走自己 L4 栈,因为 L4 只有 TCP)
package dns

import (
	"context"
	"errors"
	"fmt"
	"math/rand"
	"net"
	"strings"
	"sync"
	"time"

	"mininet/pkg/common"
)

// 根域名服务器 (RFC-defined,公网可直接 UDP:53)
// 全 13 个,随便选一个用;教学只用一个
var rootServers = []string{
	"198.41.0.4:53",   // a.root-servers.net
	"199.9.14.201:53", // b.root-servers.net
	"192.33.4.12:53",  // c.root-servers.net
}

// Resolver 递归解析器
type Resolver struct {
	timeout time.Duration
	cache   *Cache

	// 可自定义的根服务器列表 (默认用公网根)
	// 测试/本地演示时可换成 fake server
	Roots []string

	// singleflight: 同一域名的多个并发查询合并为一次
	sfMu sync.Mutex
	sf   map[string]*inflight
}

type inflight struct {
	done chan struct{}
	ips  []net.IP
	err  error
}

func NewResolver(timeout time.Duration, cacheTTL time.Duration) *Resolver {
	return &Resolver{
		timeout: timeout,
		cache:   NewCache(cacheTTL),
		sf:      make(map[string]*inflight),
	}
}

// Resolve 查询 name 的 A 记录,返回所有 IPv4
func (r *Resolver) Resolve(ctx context.Context, name string) ([]net.IP, error) {
	name = strings.TrimSuffix(strings.ToLower(name), ".")

	// 1. 查缓存
	if ips, ok := r.cache.Get(name); ok {
		common.Trace("dns", "cache hit %s → %v", name, ips)
		return ips, nil
	}

	// 2. singleflight 合并同域名并发
	r.sfMu.Lock()
	if inf, ok := r.sf[name]; ok {
		r.sfMu.Unlock()
		<-inf.done
		return inf.ips, inf.err
	}
	inf := &inflight{done: make(chan struct{})}
	r.sf[name] = inf
	r.sfMu.Unlock()

	// 3. 真正递归解析
	ips, err := r.doRecursive(ctx, name)
	inf.ips = ips
	inf.err = err
	close(inf.done)

	// 4. 清 singleflight;写缓存
	r.sfMu.Lock()
	delete(r.sf, name)
	r.sfMu.Unlock()
	if err == nil && len(ips) > 0 {
		r.cache.Set(name, ips)
	}
	return ips, err
}

// doRecursive 从根服务器开始递归
func (r *Resolver) doRecursive(ctx context.Context, name string) ([]net.IP, error) {
	// 从根开始 (可自定义)
	roots := r.Roots
	if len(roots) == 0 {
		roots = rootServers
	}
	server := roots[rand.Intn(len(roots))]
	maxHops := 10
	for hop := 0; hop < maxHops; hop++ {
		common.Info("dns", "query %s @ %s (hop %d)", name, server, hop)
		msg, err := r.queryOnce(ctx, server, name)
		if err != nil {
			return nil, fmt.Errorf("hop %d (%s): %w", hop, server, err)
		}

		// 有 Answer → 完成
		var ips []net.IP
		for _, rr := range msg.Answers {
			if rr.Type == TypeA && len(rr.Data) == 4 {
				ips = append(ips, net.IP(rr.Data))
			}
			if rr.Type == TypeCNAME {
				// 简化: 拿到 CNAME 后对别名递归(不展开解码,直接重走)
				cname, _, err := decodeName(nil, 0) // 占位
				_ = cname
				_ = err
			}
		}
		if len(ips) > 0 {
			return ips, nil
		}

		// 无 Answer,看 Authority + Additional 找下一跳 NS
		nextServer := pickNextServer(msg)
		if nextServer == "" {
			return nil, errors.New("no next NS in authority/additional")
		}
		server = nextServer
	}
	return nil, errors.New("max hops exceeded")
}

// pickNextServer 从 Authority/Additional 里找下一跳 NS 的 IP
// 简化:优先用 Additional 里 NS 对应的 A 记录;否则返回 ""
func pickNextServer(msg *Message) string {
	// 收集 Authority 里的 NS 名字
	nsNames := make(map[string]bool)
	for _, rr := range msg.Authorities {
		if rr.Type == TypeNS {
			// Data 是 "压缩的 NS 名字",需要解码
			// 简化:很多 glue 把 NS 的 IP 直接放 Additional
			_ = rr
		}
	}
	_ = nsNames
	// 从 Additional 挑任意一个 A 记录即可
	for _, rr := range msg.Additionals {
		if rr.Type == TypeA && len(rr.Data) == 4 {
			ip := net.IP(rr.Data)
			return ip.String() + ":53"
		}
	}
	return ""
}

// queryOnce 向一个 server 发一次 UDP 查询
func (r *Resolver) queryOnce(ctx context.Context, server, name string) (*Message, error) {
	conn, err := net.Dial("udp", server)
	if err != nil {
		return nil, err
	}
	defer conn.Close()

	deadline, _ := ctx.Deadline()
	if deadline.IsZero() {
		deadline = time.Now().Add(r.timeout)
	}
	conn.SetDeadline(deadline)

	q := NewQuery(uint16(rand.Uint32()), name, TypeA, false) // 迭代查询不要 RD
	buf, err := q.Encode()
	if err != nil {
		return nil, err
	}
	if _, err := conn.Write(buf); err != nil {
		return nil, err
	}

	rbuf := make([]byte, 1500)
	n, err := conn.Read(rbuf)
	if err != nil {
		return nil, err
	}
	return Decode(rbuf[:n])
}

// -------- 简单 TCP/UDP 外部 DNS 查询(给 mnet-dig 用) --------

// QueryServer 直接向指定 DNS 服务器查一次 (非递归,RD=true 让服务器递归)
// 用于 ISP DNS 或 8.8.8.8
func QueryServer(ctx context.Context, server, name string, qtype Type) (*Message, error) {
	conn, err := net.Dial("udp", server)
	if err != nil {
		return nil, err
	}
	defer conn.Close()

	deadline, _ := ctx.Deadline()
	if deadline.IsZero() {
		deadline = time.Now().Add(3 * time.Second)
	}
	conn.SetDeadline(deadline)

	q := NewQuery(uint16(rand.Uint32()), name, qtype, true) // RD=true
	buf, err := q.Encode()
	if err != nil {
		return nil, err
	}
	if _, err := conn.Write(buf); err != nil {
		return nil, err
	}
	rbuf := make([]byte, 1500)
	n, err := conn.Read(rbuf)
	if err != nil {
		return nil, err
	}
	return Decode(rbuf[:n])
}
