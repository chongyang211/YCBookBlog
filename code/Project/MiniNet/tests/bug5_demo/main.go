// tests/bug5_demo/main.go - BUG-5 缓存击穿 (Cache Stampede) 现场
//
// 跑法: make bug5-demo
//
// 场景: 热门商品详情页 /product/1
//   秒杀开始瞬间 1000 QPS 同时打到这个 URL
//   第 1 次 miss → 查数据库 50ms
//   在这 50ms 内, 另外 999 个请求也都 miss (缓存还没写入)
//   → 1000 次全穿透到源站, 数据库瞬间 100× QPS → 挂!
//
// 修复: singleflight (99 个并发等那 1 个真实查询)
package main

import (
	"fmt"
	"net"
	"sync"
	"sync/atomic"
	"time"

	"mininet/pkg/common"
	mhttp "mininet/pkg/http"
	"mininet/pkg/proxy"
)

func main() {
	common.SetLogLevel(common.LvWarn)
	fmt.Println("================================================================")
	fmt.Println("🔥 BUG-5 现场: 缓存击穿 (Cache Stampede)")
	fmt.Println("   场景: 1000 并发打同一个 URL,  源站 50ms 延迟")
	fmt.Println("================================================================")

	// 一个"数据库源站": 每次请求 50ms 延迟 + 计数
	var dbQPS int32
	originMux := mhttp.NewMux()
	originMux.HandlePrefix("GET", "/", func(w mhttp.ResponseWriter, r *mhttp.Request) {
		atomic.AddInt32(&dbQPS, 1)
		time.Sleep(50 * time.Millisecond) // 模拟数据库慢查询
		mhttp.WriteString(w, 200, "product:1 price:99.00")
	})
	originLn, _ := net.Listen("tcp", "127.0.0.1:0")
	originSrv := mhttp.NewServer("", originMux)
	go originSrv.Serve(originLn)
	defer originSrv.Shutdown()
	upstream := "http://" + originLn.Addr().String()
	time.Sleep(20 * time.Millisecond)

	// -------- 场景 A: 坏版本 (只有 LRU, 无 singleflight) --------
	run := func(name string, withSF bool, concurrent int) {
		fmt.Printf("\n----- %s (并发=%d) -----\n", name, concurrent)
		atomic.StoreInt32(&dbQPS, 0)
		p := proxy.New(upstream).WithCache(100)
		if withSF {
			p = p.WithGroup()
		}
		edgeMux := mhttp.NewMux()
		edgeMux.HandlePrefix("GET", "/", p.ServeHTTP)
		ln, _ := net.Listen("tcp", "127.0.0.1:0")
		srv := mhttp.NewServer("", edgeMux)
		go srv.Serve(ln)
		defer srv.Shutdown()
		time.Sleep(20 * time.Millisecond)

		c := &mhttp.Client{Timeout: 10 * time.Second}
		addr := "http://" + ln.Addr().String() + "/product/1"

		t0 := time.Now()
		var wg sync.WaitGroup
		for i := 0; i < concurrent; i++ {
			wg.Add(1)
			go func() {
				defer wg.Done()
				c.Get(addr)
			}()
		}
		wg.Wait()
		elapsed := time.Since(t0)

		db := atomic.LoadInt32(&dbQPS)
		amp := float64(db) / float64(concurrent) * 100
		fmt.Printf("  源站真实调用: %d 次 (放大率 %.1f%%)\n", db, amp)
		fmt.Printf("  总耗时:       %v\n", elapsed)
		fmt.Printf("  edge 统计:   %s\n", p.Stats())
		time.Sleep(50 * time.Millisecond)
	}

	run("🔥 BUG 现场:只有缓存无防击穿", false, 1000)
	run("✅ 修复:缓存 + singleflight    ", true, 1000)

	fmt.Println()
	fmt.Println("================================================================")
	fmt.Println("📊 结论:")
	fmt.Println("   坏版本: 1000 并发 → 源站~1000 次 QPS (击穿! 数据库会挂)")
	fmt.Println("   修复后: 1000 并发 → 源站 1 次 QPS  (999 个请求等那 1 个)")
	fmt.Println("   放大率从 ~100% 降到 ~0.1%,相当于 1000× 保护")
	fmt.Println("================================================================")
}
