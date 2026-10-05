// cmd/mnet/main.go - MiniNet 主程序 (阶段 ①-② 版本)
//
// REPL 命令:
//
//	dump                                     打印 5 层协议栈当前状态
//	route add <cidr> via <nh> [dev <iface>]  添加路由
//	route show                               打印路由表
//	route lookup <ip>                        最长前缀匹配
//	help                                     命令帮助
//	quit / exit / Ctrl-D                     退出
package main

import (
	"bufio"
	"flag"
	"fmt"
	"os"
	"strings"

	"mininet/pkg/common"
	netpkg "mininet/pkg/net"
)

// -------- Layer 接口 (全项目核心抽象) --------

// Layer 代表协议栈某一层的占位
// 后续阶段每一层(ARP/IP/TCP/HTTP)都会实现这个接口
type Layer interface {
	Name() string // 层名,如 "L4-TCP"
	Dump() string // 一行字符串,打印到 >>> dump 里
}

// EmptyLayer 占位实现:还没写的层,现在都用它
// (显式空占位比 nil 安全,避免 >>> dump 时 nil 解引用 panic)
type EmptyLayer struct{ name string }

func (e *EmptyLayer) Name() string { return e.name }
func (e *EmptyLayer) Dump() string { return "[Empty]" }

// -------- Stack 五层容器 --------

type Stack struct {
	L2     Layer // 链路层    阶段 ④ 填: Ethernet + ARP
	L3     Layer // 网络层    阶段 ⑤ 填: IPv4 + ICMP
	L4     Layer // 传输层    阶段 ⑥ 填: TCP (阶段 ⑨ 加 UDP)
	L7     Layer // 应用层    阶段 ⑪ 填: HTTP
	App    Layer // 业务层    阶段 ⑯ 填: shop demo
	Routes *netpkg.RouteTable
}

func newStack() *Stack {
	// 一开始全是空占位,后续阶段替换为真实实现
	return &Stack{
		L2:     &EmptyLayer{name: "L2-Link"},
		L3:     &EmptyLayer{name: "L3-Net"},
		L4:     &EmptyLayer{name: "L4-Transport"},
		L7:     &EmptyLayer{name: "L7-App"},
		App:    &EmptyLayer{name: "App-Business"},
		Routes: netpkg.NewRouteTable(),
	}
}

func (s *Stack) dump() {
	fmt.Println("┌─── MiniNet Stack ─────────────────────────┐")
	for _, l := range []Layer{s.L2, s.L3, s.L4, s.L7, s.App} {
		fmt.Printf("│  %-14s : %s\n", l.Name(), l.Dump())
	}
	fmt.Println("└───────────────────────────────────────────┘")
}

// -------- REPL 主循环 --------

func main() {
	// 命令行 flag
	logLv := flag.String("log", "info", "log level: trace/info/warn/error")
	flag.Parse()

	lv, err := common.ParseLogLevel(*logLv)
	if err != nil {
		fmt.Fprintln(os.Stderr, err)
		os.Exit(1)
	}
	common.SetLogLevel(lv)
	common.Info("boot", "MiniNet starting with log=%s", *logLv)

	fmt.Println("MiniNet v0.1 (Ctrl-D or `quit` to exit)")
	stack := newStack()

	reader := bufio.NewReader(os.Stdin)
	for {
		fmt.Print(">>> ")
		line, err := reader.ReadString('\n')
		if err != nil { // Ctrl-D / EOF
			fmt.Println()
			break
		}
		line = strings.TrimSpace(line)
		if line == "" {
			continue
		}
		parts := strings.Fields(line)
		cmd := parts[0]

		switch cmd {
		case "dump":
			stack.dump()
		case "route":
			handleRoute(stack, parts[1:])
		case "help":
			printHelp()
		case "quit", "exit":
			fmt.Println("bye.")
			return
		default:
			fmt.Printf("(error) unknown command: %s (try 'help')\n", cmd)
		}
	}
	fmt.Println("bye.")
}

func printHelp() {
	fmt.Println("commands:")
	fmt.Println("  dump                                     打印 5 层协议栈当前状态")
	fmt.Println("  route add <cidr> via <nh> [dev <iface>]  添加路由")
	fmt.Println("  route show                               打印路由表")
	fmt.Println("  route lookup <ip>                        最长前缀匹配")
	fmt.Println("  help                                     命令帮助")
	fmt.Println("  quit / exit                              退出")
}

// handleRoute 处理 >>> route add/show/lookup 命令
func handleRoute(s *Stack, args []string) {
	if len(args) == 0 {
		fmt.Println("usage: route add <cidr> via <nexthop> [dev <iface>]")
		fmt.Println("       route show")
		fmt.Println("       route lookup <ip>")
		return
	}
	switch args[0] {
	case "add":
		// route add 10.0.0.0/8 via 10.0.0.1 dev lan1
		if len(args) < 4 || args[2] != "via" {
			fmt.Println("usage: route add <cidr> via <nexthop> [dev <iface>]")
			return
		}
		sub, err := netpkg.ParseCIDR(args[1])
		if err != nil {
			fmt.Println("(error)", err)
			return
		}
		nh, err := netpkg.ParseIPv4(args[3])
		if err != nil {
			fmt.Println("(error)", err)
			return
		}
		iface := ""
		if len(args) >= 6 && args[4] == "dev" {
			iface = args[5]
		}
		s.Routes.Add(netpkg.Route{Dst: sub, NextHop: nh, Iface: iface})
		fmt.Println("route added")

	case "show":
		fmt.Print(s.Routes.Show())

	case "lookup":
		if len(args) < 2 {
			fmt.Println("usage: route lookup <ip>")
			return
		}
		ip, err := netpkg.ParseIPv4(args[1])
		if err != nil {
			fmt.Println("(error)", err)
			return
		}
		r, ok := s.Routes.Lookup(ip)
		if !ok {
			fmt.Printf("no route to %s\n", ip)
			return
		}
		iface := r.Iface
		if iface == "" {
			iface = "-"
		}
		fmt.Printf("match %s via %s dev %s (prefix=%d)\n",
			r.Dst, r.NextHop, iface, r.Dst.Prefix)
	default:
		fmt.Println("unknown route subcommand:", args[0])
	}
}
