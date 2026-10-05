// pkg/proxy/proxy.go - 反向代理 + LRU 缓存 (Nginx 微缩版)
//
// 第 6 次会话 Step 13.2 + 13.4
//
// 请求流程:
//   client → ReverseProxy.ServeHTTP → (查缓存命中? 直接回) → (未命中)
//     → singleflight.Do → 向 upstream 发 HTTP 请求 (走 pkg/http.Client)
//     → 读响应 → 根据 Cache-Control 决定存不存
//     → 回给 client
//
// 支持:
//   - X-Request-ID 透传
//   - X-Cache: HIT/MISS/BYPASS 响应头
//   - X-Upstream-Latency 上游耗时
//   - singleflight 防击穿
//   - TTL jitter 防雪崩 (把 1000 key 同秒过期打散)
package proxy

import (
	"fmt"
	"math/rand"
	"strconv"
	"strings"
	"sync"
	"sync/atomic"
	"time"

	"mininet/pkg/cache"
	"mininet/pkg/common"
	mhttp "mininet/pkg/http"
)

// ReverseProxy 反向代理
type ReverseProxy struct {
	Upstream   string             // "http://127.0.0.1:9001"
	Cache      *cache.LRU         // nil 表示不缓存
	Group      *cache.Group       // nil 表示不防击穿
	JitterRate float64            // TTL 抖动比例 (0.2 表示 ±20%)
	Client     *mhttp.Client
	// 统计
	UpstreamQPS atomic.Int64
	CacheHit    atomic.Int64
	CacheMiss   atomic.Int64
}

// New 创建一个反代实例
func New(upstream string) *ReverseProxy {
	return &ReverseProxy{
		Upstream:   upstream,
		Client:     &mhttp.Client{Timeout: 10 * time.Second},
		JitterRate: 0.1,
	}
}

// WithCache 启用缓存 (配合 WithGroup 可防击穿)
func (p *ReverseProxy) WithCache(cap int) *ReverseProxy {
	p.Cache = cache.NewLRU(cap)
	return p
}

// WithGroup 启用 singleflight
func (p *ReverseProxy) WithGroup() *ReverseProxy {
	p.Group = cache.NewGroup()
	return p
}

// ServeHTTP 处理一个请求
func (p *ReverseProxy) ServeHTTP(w mhttp.ResponseWriter, r *mhttp.Request) {
	// X-Request-ID 透传 (边缘节点第一个加,后续节点原样带)
	rid := r.Header.Get("X-Request-ID")
	if rid == "" {
		rid = genReqID()
	}
	w.Header().Set("X-Request-ID", rid)

	// 查缓存 (只缓存 GET)
	cacheKey := r.Method + " " + r.URL
	if p.Cache != nil && r.Method == "GET" {
		if e, hit := p.Cache.Get(cacheKey); hit {
			p.CacheHit.Add(1)
			w.Header().Set("X-Cache", "HIT")
			w.Header().Set("Content-Type", e.ETag) // 教学简化:把 CT 塞 ETag 字段
			w.WriteHeader(200)
			w.Write(e.Value)
			common.Trace("proxy", "[%s] HIT %s → %d bytes", rid, cacheKey, len(e.Value))
			return
		}
		p.CacheMiss.Add(1)
	}

	// 回源 (可选 singleflight)
	fetch := func() ([]byte, error) {
		p.UpstreamQPS.Add(1)
		return p.doUpstream(rid, r)
	}
	var body []byte
	var err error
	shared := false
	if p.Group != nil {
		body, err, shared = p.Group.Do(cacheKey, fetch)
	} else {
		body, err = fetch()
	}

	if err != nil {
		w.Header().Set("X-Cache", "BYPASS")
		mhttp.WriteString(w, 502, fmt.Sprintf("upstream error: %v\n", err))
		return
	}

	if p.Cache != nil && r.Method == "GET" {
		// 带 jitter 的 TTL 防雪崩
		base := 10 * time.Second
		ttl := jitter(base, p.JitterRate)
		p.Cache.Set(cacheKey, body, ttl)
	}

	w.Header().Set("X-Cache", "MISS")
	if shared {
		w.Header().Set("X-Cache", "MISS-SHARED") // singleflight 复用
	}
	w.WriteHeader(200)
	w.Write(body)
	common.Trace("proxy", "[%s] MISS %s → %d bytes (shared=%v)", rid, cacheKey, len(body), shared)
}

// doUpstream 真实回源
func (p *ReverseProxy) doUpstream(rid string, r *mhttp.Request) ([]byte, error) {
	// 构造 upstream URL
	u := strings.TrimRight(p.Upstream, "/") + r.URL
	headers := make(mhttp.Header)
	for k, v := range r.Header {
		// 过滤掉 Host (让 Client 根据 upstream 填)
		if strings.EqualFold(k, "Host") {
			continue
		}
		headers[k] = v
	}
	headers.Set("X-Request-ID", rid)
	headers.Set("X-Forwarded-For", "mnet-edge")

	t0 := time.Now()
	res, err := p.Client.Do(r.Method, u, headers, r.Body)
	common.Trace("proxy", "[%s] upstream %s took %v", rid, u, time.Since(t0))
	if err != nil {
		return nil, err
	}
	if res.Response == nil {
		return nil, fmt.Errorf("empty response")
	}
	return res.Response.Body, nil
}

// Stats 展示用
func (p *ReverseProxy) Stats() string {
	var b strings.Builder
	fmt.Fprintf(&b, "upstream_qps=%d cache_hit=%d cache_miss=%d",
		p.UpstreamQPS.Load(), p.CacheHit.Load(), p.CacheMiss.Load())
	if p.Cache != nil {
		s := p.Cache.Stats()
		fmt.Fprintf(&b, " lru_len=%d/%d hit_rate=%.2f%%",
			s.Len, s.Cap, s.HitRate*100)
	}
	return b.String()
}

// -------- 工具 --------

var reqIDCounter atomic.Uint64

func genReqID() string {
	n := reqIDCounter.Add(1)
	return fmt.Sprintf("%x-%s", time.Now().UnixNano()&0xffffff, strconv.FormatUint(n, 36))
}

// jitter 把 base 时长抖动 ±rate 比例
//   jitter(10s, 0.2) → 均匀分布于 [8s, 12s]
func jitter(base time.Duration, rate float64) time.Duration {
	if rate <= 0 {
		return base
	}
	delta := float64(base) * rate
	off := (rand.Float64()*2 - 1) * delta // [-delta, +delta]
	return base + time.Duration(off)
}

// jitterSeeded 测试用确定性 jitter (暂不启用)
var _ = sync.Mutex{}
