// cmd/mnet-dig/main.go - 独立 DNS 查询工具 (类似 dig)
//
// 用法:
//
//   ./bin/mnet-dig www.example.com                 # 向本地默认 DNS (8.8.8.8) 查
//   ./bin/mnet-dig @1.1.1.1 www.example.com        # 向指定 DNS 查
//   ./bin/mnet-dig -recursive www.example.com      # 从根开始自己递归 (教学模式)
//
// 输出格式参考真 dig,方便和 dig 对拍。
package main

import (
	"context"
	"flag"
	"fmt"
	"net"
	"os"
	"strings"
	"time"

	"mininet/pkg/common"
	"mininet/pkg/dns"
)

func main() {
	recursive := flag.Bool("recursive", false, "do our own recursion from root servers (教学模式)")
	rootArg := flag.String("root", "", "override root DNS server (for -recursive, offline testing)")
	logLv := flag.String("log", "warn", "log level")
	flag.Usage = func() {
		fmt.Fprintln(os.Stderr, "usage: mnet-dig [flags] [@server] <name>")
		fmt.Fprintln(os.Stderr, "  default server = 8.8.8.8 (Google DNS)")
		flag.PrintDefaults()
	}
	flag.Parse()

	lv, _ := common.ParseLogLevel(*logLv)
	common.SetLogLevel(lv)

	// 解析位置参数: @server 和 name
	server := "8.8.8.8:53"
	var name string
	for _, arg := range flag.Args() {
		if strings.HasPrefix(arg, "@") {
			server = arg[1:]
			if !strings.Contains(server, ":") {
				server += ":53"
			}
		} else {
			name = arg
		}
	}
	if name == "" {
		flag.Usage()
		os.Exit(2)
	}

	t0 := time.Now()
	ctx, cancel := context.WithTimeout(context.Background(), 10*time.Second)
	defer cancel()

	if *recursive {
		runRecursive(ctx, name, *rootArg, t0)
	} else {
		runDirect(ctx, server, name, t0)
	}
}

func runDirect(ctx context.Context, server, name string, t0 time.Time) {
	fmt.Printf("; <<>> mnet-dig <<>> @%s %s\n", server, name)
	msg, err := dns.QueryServer(ctx, server, name, dns.TypeA)
	if err != nil {
		fmt.Fprintln(os.Stderr, "query failed:", err)
		os.Exit(1)
	}
	printResult(msg, server, time.Since(t0))
}

func runRecursive(ctx context.Context, name, rootOverride string, t0 time.Time) {
	fmt.Printf("; <<>> mnet-dig <<>> -recursive %s\n", name)
	r := dns.NewResolver(5*time.Second, 60*time.Second)
	if rootOverride != "" {
		if !strings.Contains(rootOverride, ":") {
			rootOverride += ":53"
		}
		r.Roots = []string{rootOverride}
		fmt.Printf(";; (recursing from %s)\n", rootOverride)
	} else {
		fmt.Println(";; (recursing from public root servers)")
	}
	ips, err := r.Resolve(ctx, name)
	if err != nil {
		fmt.Fprintln(os.Stderr, "recursive resolve failed:", err)
		os.Exit(1)
	}
	fmt.Println()
	fmt.Println(";; ANSWER SECTION:")
	for _, ip := range ips {
		fmt.Printf("%-30s IN  A  %s\n", name+".", ip)
	}
	fmt.Printf("\n;; Query time: %v\n", time.Since(t0))
}

func printResult(msg *dns.Message, server string, elapsed time.Duration) {
	fmt.Printf(";; got response from %s\n", server)
	fmt.Printf(";; HEADER: id=%d flags=0x%04x rcode=%d\n",
		msg.Header.ID, msg.Header.Flags, msg.Header.RCode())
	fmt.Printf(";; qd=%d an=%d ns=%d ar=%d\n",
		msg.Header.QDCount, msg.Header.ANCount,
		msg.Header.NSCount, msg.Header.ARCount)

	if len(msg.Questions) > 0 {
		fmt.Println()
		fmt.Println(";; QUESTION SECTION:")
		for _, q := range msg.Questions {
			fmt.Printf(";%-28s IN  %s\n", q.Name+".", q.Type)
		}
	}
	if len(msg.Answers) > 0 {
		fmt.Println()
		fmt.Println(";; ANSWER SECTION:")
		for _, rr := range msg.Answers {
			printRR(rr)
		}
	}
	if len(msg.Authorities) > 0 {
		fmt.Println()
		fmt.Println(";; AUTHORITY SECTION:")
		for _, rr := range msg.Authorities {
			printRR(rr)
		}
	}
	fmt.Printf("\n;; Query time: %v\n", elapsed)
}

func printRR(rr dns.RR) {
	val := ""
	switch rr.Type {
	case dns.TypeA:
		if len(rr.Data) == 4 {
			val = net.IP(rr.Data).String()
		}
	case dns.TypeAAAA:
		if len(rr.Data) == 16 {
			val = net.IP(rr.Data).String()
		}
	default:
		val = fmt.Sprintf("(%d bytes)", len(rr.Data))
	}
	fmt.Printf("%-30s %d  IN  %-5s %s\n", rr.Name+".", rr.TTL, rr.Type, val)
}
