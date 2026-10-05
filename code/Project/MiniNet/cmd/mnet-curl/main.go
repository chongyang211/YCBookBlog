// cmd/mnet-curl/main.go - 独立 HTTP(S) 客户端工具 (类似 curl)
//
// 用法:
//
//   ./bin/mnet-curl http://example.com/
//   ./bin/mnet-curl -v https://www.example.com/          # verbose
//   ./bin/mnet-curl --tls-debug https://www.example.com/ # 打印 ClientHello 字节
//   ./bin/mnet-curl -X POST -d 'hello' http://localhost:8080/echo
//   ./bin/mnet-curl -k https://self-signed.local/        # 跳过证书校验
package main

import (
	"flag"
	"fmt"
	"os"
	"strings"
	"time"

	"mininet/pkg/common"
	mhttp "mininet/pkg/http"
	mtls "mininet/pkg/tls"
)

func main() {
	method := flag.String("X", "GET", "HTTP method")
	data := flag.String("d", "", "request body")
	verbose := flag.Bool("v", false, "verbose (print req/resp headers)")
	tlsDebug := flag.Bool("tls-debug", false, "capture + decode ClientHello before real request")
	insecure := flag.Bool("k", false, "skip TLS cert verify")
	logLv := flag.String("log", "warn", "log level")
	flag.Usage = func() {
		fmt.Fprintln(os.Stderr, "usage: mnet-curl [flags] <url>")
		flag.PrintDefaults()
	}
	flag.Parse()
	if flag.NArg() != 1 {
		flag.Usage()
		os.Exit(2)
	}
	rawURL := flag.Arg(0)

	lv, _ := common.ParseLogLevel(*logLv)
	common.SetLogLevel(lv)

	// --tls-debug: 用 CaptureClientHello 跑一次,打印字节分布
	if *tlsDebug && strings.HasPrefix(rawURL, "https://") {
		host := extractHost(rawURL)
		fmt.Printf("===== 🔍 TLS Debug: capturing ClientHello for %s =====\n", host)
		hello, raw, err := mtls.CaptureClientHello(host, []string{"h2", "http/1.1"})
		if err != nil {
			fmt.Fprintln(os.Stderr, "capture failed:", err)
		} else {
			fmt.Printf("Raw ClientHello: %d bytes\n", len(raw))
			fmt.Print(hello.Describe())
			fmt.Println()
		}
	}

	c := &mhttp.Client{
		Timeout:            10 * time.Second,
		Verbose:            *verbose,
		InsecureSkipVerify: *insecure,
	}

	var headers mhttp.Header
	var body []byte
	if *data != "" {
		headers = mhttp.Header{"Content-Type": "application/x-www-form-urlencoded"}
		body = []byte(*data)
		if *method == "GET" {
			*method = "POST"
		}
	}

	res, err := c.Do(*method, rawURL, headers, body)
	if err != nil {
		fmt.Fprintln(os.Stderr, "error:", err)
		os.Exit(1)
	}

	// TLS 握手信息
	if res.TLSInfo != nil && *verbose {
		fmt.Println()
		fmt.Print(res.TLSInfo.String())
		fmt.Println()
	}

	// 响应 body
	if res.Response != nil {
		os.Stdout.Write(res.Response.Body)
		if len(res.Response.Body) > 0 && res.Response.Body[len(res.Response.Body)-1] != '\n' {
			fmt.Println()
		}
	}

	if *verbose {
		fmt.Fprintf(os.Stderr, "\n;; Total time: %v\n", res.Elapsed)
	}
}

func extractHost(rawURL string) string {
	s := strings.TrimPrefix(rawURL, "https://")
	s = strings.TrimPrefix(s, "http://")
	if idx := strings.IndexAny(s, "/?"); idx >= 0 {
		s = s[:idx]
	}
	if idx := strings.Index(s, ":"); idx >= 0 {
		s = s[:idx]
	}
	return s
}
