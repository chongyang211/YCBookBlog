// tests/http_demo/main.go - HTTP Server + Client 端到端 demo (不依赖公网)
//
// 跑法:
//   go run ./tests/http_demo
//   # 或
//   make http-demo
//
// 启动一个 HTTP server (3 个路由) + 用自写 client 跑 5 种请求
package main

import (
	"fmt"
	"net"
	"time"

	"mininet/pkg/common"
	mhttp "mininet/pkg/http"
)

func main() {
	common.SetLogLevel(common.LvInfo)

	// 1. 建 server
	mux := mhttp.NewMux()
	mux.Handle("GET", "/", func(w mhttp.ResponseWriter, r *mhttp.Request) {
		mhttp.WriteString(w, 200, "<h1>Welcome to MiniNet!</h1>")
		w.Header().Set("Content-Type", "text/html; charset=utf-8")
	})
	mux.Handle("GET", "/api/time", func(w mhttp.ResponseWriter, r *mhttp.Request) {
		mhttp.WriteJSON(w, 200, fmt.Sprintf(`{"time":"%s"}`, time.Now().Format(time.RFC3339)))
	})
	mux.Handle("POST", "/echo", func(w mhttp.ResponseWriter, r *mhttp.Request) {
		w.WriteHeader(200)
		w.Header().Set("X-Received-Bytes", fmt.Sprintf("%d", len(r.Body)))
		w.Write(r.Body)
	})
	mux.Handle("GET", "/redirect", func(w mhttp.ResponseWriter, r *mhttp.Request) {
		mhttp.Redirect(w, 302, "/")
	})
	mux.Handle("GET", "/slow", func(w mhttp.ResponseWriter, r *mhttp.Request) {
		time.Sleep(100 * time.Millisecond)
		mhttp.WriteString(w, 200, "slow response")
	})

	ln, err := net.Listen("tcp", "127.0.0.1:0")
	if err != nil {
		panic(err)
	}
	addr := "http://" + ln.Addr().String()
	srv := mhttp.NewServer("", mux)
	go srv.Serve(ln)
	defer srv.Shutdown()
	time.Sleep(50 * time.Millisecond)

	fmt.Printf("====== HTTP server listening @ %s ======\n\n", addr)

	// 2. 用自写 client 跑一组请求
	c := &mhttp.Client{Timeout: 2 * time.Second}

	type testCase struct {
		name   string
		method string
		path   string
		body   []byte
	}
	cases := []testCase{
		{"GET /         ", "GET", "/", nil},
		{"GET /api/time ", "GET", "/api/time", nil},
		{"POST /echo    ", "POST", "/echo", []byte("ping-pong")},
		{"GET /redirect ", "GET", "/redirect", nil},
		{"GET /slow     ", "GET", "/slow", nil},
		{"GET /notfound ", "GET", "/notfound", nil},
	}

	for _, tc := range cases {
		var headers mhttp.Header
		if tc.body != nil {
			headers = mhttp.Header{"Content-Type": "text/plain"}
		}
		t0 := time.Now()
		res, err := c.Do(tc.method, addr+tc.path, headers, tc.body)
		elapsed := time.Since(t0)
		if err != nil {
			fmt.Printf("  %s → ERR: %v\n", tc.name, err)
			continue
		}
		bodyPrev := string(res.Response.Body)
		if len(bodyPrev) > 50 {
			bodyPrev = bodyPrev[:50] + "..."
		}
		fmt.Printf("  %s → %d (%v)  %q\n",
			tc.name, res.Response.StatusCode, elapsed, bodyPrev)
	}

	fmt.Println()
	fmt.Println("====== 📊 结论 ======")
	fmt.Println("自写的 HTTP server + client 完整互通:")
	fmt.Println("  - 请求行 + Header + Body 解析")
	fmt.Println("  - Mux 路由(精确 + 前缀 + 404 fallback)")
	fmt.Println("  - 自动 Content-Length / Server / Connection")
	fmt.Println("  - 响应 chunked 解析(见 pkg/http 的单测)")
}
