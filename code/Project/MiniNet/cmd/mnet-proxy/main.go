// cmd/mnet-proxy/main.go - 独立反向代理二进制
//
// 用法:
//   mnet-proxy -listen :8080 -upstream http://127.0.0.1:9000
//   mnet-proxy -listen :8080 -upstream http://x.com -cache 1000 -singleflight
package main

import (
	"flag"
	"fmt"
	"os"
	"os/signal"
	"syscall"
	"time"

	"mininet/pkg/common"
	mhttp "mininet/pkg/http"
	"mininet/pkg/proxy"
)

func main() {
	listen := flag.String("listen", ":8080", "listen address")
	upstream := flag.String("upstream", "", "upstream URL, e.g. http://127.0.0.1:9000")
	cacheCap := flag.Int("cache", 0, "LRU capacity (0=no cache)")
	singleFlight := flag.Bool("singleflight", false, "enable singleflight")
	logLv := flag.String("log", "info", "log level")
	statsEvery := flag.Duration("stats", 5*time.Second, "print stats period")
	flag.Parse()

	if *upstream == "" {
		fmt.Fprintln(os.Stderr, "ERROR: -upstream required")
		flag.Usage()
		os.Exit(2)
	}
	lv, _ := common.ParseLogLevel(*logLv)
	common.SetLogLevel(lv)

	p := proxy.New(*upstream)
	if *cacheCap > 0 {
		p.WithCache(*cacheCap)
	}
	if *singleFlight {
		p.WithGroup()
	}

	mux := mhttp.NewMux()
	mux.HandlePrefix("GET", "/", p.ServeHTTP)
	mux.HandlePrefix("POST", "/", p.ServeHTTP)
	srv := mhttp.NewServer(*listen, mux)
	srv.KeepAlive = true

	fmt.Printf("mnet-proxy listening on %s → %s\n", *listen, *upstream)
	fmt.Printf("  cache=%d singleflight=%v\n", *cacheCap, *singleFlight)

	// 定期打统计
	go func() {
		if *statsEvery <= 0 {
			return
		}
		tk := time.NewTicker(*statsEvery)
		defer tk.Stop()
		for range tk.C {
			fmt.Printf("[STATS] %s\n", p.Stats())
		}
	}()

	// 优雅关闭
	go func() {
		sig := make(chan os.Signal, 1)
		signal.Notify(sig, syscall.SIGINT, syscall.SIGTERM)
		<-sig
		fmt.Println("\nshutting down...")
		srv.Shutdown()
	}()

	if err := srv.ListenAndServe(); err != nil {
		fmt.Fprintln(os.Stderr, "server error:", err)
		os.Exit(1)
	}
}
