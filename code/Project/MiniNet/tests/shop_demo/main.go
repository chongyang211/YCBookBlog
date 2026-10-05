// tests/shop_demo/main.go - 阶段 ⑯ 总装: shop 全栈 demo
//
// 跑法: make demo-shop  → 打开 http://127.0.0.1:8080/
//
// 本 demo 一键启动 3 个服务 (全部用 goroutine, 省得你开 3 个终端):
//
//   浏览器:8080 ─┐
//                 │
//                 ▼  HTTP + WS 推送
//          ┌──────────────────┐
//          │ 边缘节点 :8080    │ ← mnet-proxy (LRU + singleflight)
//          │  + 静态 shop.html │
//          └──────────────────┘
//                 │
//                 ▼ HTTP (回源)
//          ┌──────────────────┐
//          │ 商品源站 :9001    │
//          └──────────────────┘
//
//          ┌──────────────────┐
//          │ WS 推送 :9100     │ ← ws://.../push
//          └──────────────────┘
//
// 把前 6 次会话的所有拼图串起来:
//   阶段 ⑩ TLS → 阶段 ⑪ HTTP server → 阶段 ⑫ Keep-Alive
//   阶段 ⑬ 反代+LRU+SF (本 demo 用) → 阶段 ⑭ WebSocket (本 demo 用)
package main

import (
	"bufio"
	"embed"
	"encoding/json"
	"fmt"
	"math/rand"
	"net"
	"os"
	"os/signal"
	"sync"
	"sync/atomic"
	"syscall"
	"time"

	"mininet/pkg/common"
	mhttp "mininet/pkg/http"
	"mininet/pkg/proxy"
	"mininet/pkg/ws"
)

//go:embed shop.html
var staticFS embed.FS

type product struct {
	ID    int    `json:"id"`
	Name  string `json:"name"`
	Price int    `json:"price"`
	Stock int    `json:"stock"`
}

var (
	productsMu sync.Mutex
	products   = []product{
		{1, "iPhone 15 Pro", 7999, 42},
		{2, "MacBook Pro M3", 14999, 15},
		{3, "AirPods Pro 2", 1899, 100},
		{4, "iPad Air", 4799, 8},
		{5, "Apple Watch S9", 2999, 23},
	}
	roomMu  sync.Mutex
	roomCon = make(map[*ws.Conn]bool)
)

func main() {
	common.SetLogLevel(common.LvInfo)
	fmt.Println("================================================================")
	fmt.Println("🛒 MiniNet Shop 全栈 Demo (阶段 ⑯ 总装)")
	fmt.Println("================================================================")

	originCalls := int32(0)

	// -------- 1. 商品源站 --------
	originMux := mhttp.NewMux()
	originMux.Handle("GET", "/api/products", func(w mhttp.ResponseWriter, r *mhttp.Request) {
		atomic.AddInt32(&originCalls, 1)
		time.Sleep(50 * time.Millisecond) // 模拟慢 DB
		productsMu.Lock()
		b, _ := json.Marshal(products)
		productsMu.Unlock()
		mhttp.WriteJSON(w, 200, string(b))
	})
	originSrv := mhttp.NewServer(":9001", originMux)
	originSrv.KeepAlive = true
	go func() {
		if err := originSrv.ListenAndServe(); err != nil {
			fmt.Println("origin err:", err)
			os.Exit(1)
		}
	}()
	fmt.Println("  [origin]  商品源站   http://127.0.0.1:9001/api/products")

	// -------- 2. 边缘节点: proxy + 静态 html --------
	rp := proxy.New("http://127.0.0.1:9001").WithCache(100).WithGroup()
	edgeMux := mhttp.NewMux()
	edgeMux.Handle("GET", "/", func(w mhttp.ResponseWriter, r *mhttp.Request) {
		data, _ := staticFS.ReadFile("shop.html")
		w.Header().Set("Content-Type", "text/html; charset=utf-8")
		w.WriteHeader(200)
		w.Write(data)
	})
	edgeMux.HandlePrefix("GET", "/api/", rp.ServeHTTP)
	edgeMux.Handle("GET", "/stats", func(w mhttp.ResponseWriter, r *mhttp.Request) {
		roomMu.Lock()
		wsN := len(roomCon)
		roomMu.Unlock()
		mhttp.WriteJSON(w, 200, fmt.Sprintf(
			`{"edge":"%s","ws_connected":%d,"origin_calls":%d}`,
			rp.Stats(), wsN, atomic.LoadInt32(&originCalls)))
	})
	edgeSrv := mhttp.NewServer(":8080", edgeMux)
	edgeSrv.KeepAlive = true
	go func() {
		if err := edgeSrv.ListenAndServe(); err != nil {
			fmt.Println("edge err:", err)
			os.Exit(1)
		}
	}()
	fmt.Println("  [edge]    边缘节点   http://127.0.0.1:8080/      (LRU+singleflight)")
	fmt.Println("  [edge]    统计端点   http://127.0.0.1:8080/stats")

	// -------- 3. WebSocket 推送 --------
	wsLn, err := net.Listen("tcp", ":9100")
	if err != nil {
		fmt.Println("ws listen:", err)
		os.Exit(1)
	}
	go func() {
		for {
			c, err := wsLn.Accept()
			if err != nil {
				return
			}
			go handleWS(c)
		}
	}()
	fmt.Println("  [push]    WS 推送    ws://127.0.0.1:9100/push")

	// 库存模拟器
	go stockSimulator()

	fmt.Println()
	fmt.Println("================================================================")
	fmt.Println("👉 浏览器打开 http://127.0.0.1:8080/")
	fmt.Println("   每 3s 刷商品列表 (观察 X-Cache HIT/MISS)")
	fmt.Println("   每 2s 推库存变化 (页面右上角实时更新)")
	fmt.Println("================================================================")

	// 定期打快照
	go func() {
		tk := time.NewTicker(5 * time.Second)
		defer tk.Stop()
		for range tk.C {
			roomMu.Lock()
			wsN := len(roomCon)
			roomMu.Unlock()
			fmt.Printf("[STATS] %s | ws_conn=%d | origin_calls=%d\n",
				rp.Stats(), wsN, atomic.LoadInt32(&originCalls))
		}
	}()

	sig := make(chan os.Signal, 1)
	signal.Notify(sig, syscall.SIGINT, syscall.SIGTERM)
	<-sig
	fmt.Println("\nshutting down...")
	edgeSrv.Shutdown()
	originSrv.Shutdown()
	wsLn.Close()
}

func handleWS(conn net.Conn) {
	br := bufio.NewReader(conn)
	req, err := mhttp.ReadRequest(br)
	if err != nil {
		conn.Close()
		return
	}
	wsc, err := ws.Upgrade(conn, br, req)
	if err != nil {
		conn.Close()
		return
	}
	wsc.StartHeartbeat(30 * time.Second)
	roomMu.Lock()
	roomCon[wsc] = true
	n := len(roomCon)
	roomMu.Unlock()
	common.Info("shop", "ws joined, now %d connected", n)

	defer func() {
		roomMu.Lock()
		delete(roomCon, wsc)
		roomMu.Unlock()
		wsc.Close()
		common.Info("shop", "ws left")
	}()
	for {
		if _, _, err := wsc.Read(); err != nil {
			return
		}
	}
}

func stockSimulator() {
	tk := time.NewTicker(2 * time.Second)
	defer tk.Stop()
	for range tk.C {
		productsMu.Lock()
		idx := rand.Intn(len(products))
		if products[idx].Stock > 0 {
			products[idx].Stock--
		} else {
			products[idx].Stock = 50
		}
		p := products[idx]
		productsMu.Unlock()

		msg := fmt.Sprintf(`{"type":"stock_update","product_id":%d,"stock":%d}`, p.ID, p.Stock)
		roomMu.Lock()
		dead := []*ws.Conn{}
		for c := range roomCon {
			if err := c.WriteText(msg); err != nil {
				dead = append(dead, c)
			}
		}
		for _, c := range dead {
			delete(roomCon, c)
		}
		roomMu.Unlock()
	}
}
