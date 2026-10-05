// cmd/mnet/main.go - MiniNet 主程序 (阶段 ①-⑤ 版本)
//
// REPL 命令:
//
//	dump                                     打印 5 层协议栈当前状态
//	route add <cidr> via <nh> [dev <iface>]  添加路由
//	route show                               打印路由表
//	route lookup <ip>                        最长前缀匹配
//	peer up                                  启动一个对端 goroutine (loopback,阶段 ④)
//	arp whois <ip>                           主动 ARP 问询 (阶段 ④)
//	arp -a                                   列出 ARP 表 (阶段 ④)
//	ping <ip> [count]                        发 N 个 ICMP echo (阶段 ⑤)
//	help / quit / exit / Ctrl-D
package main

import (
	"bufio"
	"flag"
	"fmt"
	"os"
	"strconv"
	"strings"
	"time"

	"mininet/pkg/common"
	"mininet/pkg/link"
	netpkg "mininet/pkg/net"
)

// -------- Layer 接口 (全项目核心抽象,与 pkg/link.L2Layer / pkg/net.L3Layer 结构匹配) --------

type Layer interface {
	Name() string
	Dump() string
}

type EmptyLayer struct{ name string }

func (e *EmptyLayer) Name() string { return e.name }
func (e *EmptyLayer) Dump() string { return "[Empty]" }

// -------- Stack 五层容器 --------

type Stack struct {
	L2     Layer // 阶段 ④ 填: *link.L2Layer
	L3     Layer // 阶段 ⑤ 填: *netpkg.L3Layer
	L4     Layer // 阶段 ⑥ 填: TCP
	L7     Layer // 阶段 ⑪ 填: HTTP
	App    Layer // 阶段 ⑯ 填: shop demo
	Routes *netpkg.RouteTable

	// 本机 IP/MAC (阶段 ④ peer up 时填入)
	localMAC link.MAC
	localIP  netpkg.IPv4Addr

	// 实际的 L2/L3 实例 (为了方便从 REPL 调方法)
	l2 *link.L2Layer
	l3 *netpkg.L3Layer

	// peer goroutine 的 L2/L3 (为了关闭)
	peerL2 *link.L2Layer
	peerL3 *netpkg.L3Layer
}

func newStack() *Stack {
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
	logLv := flag.String("log", "info", "log level: trace/info/warn/error")
	flag.Parse()

	lv, err := common.ParseLogLevel(*logLv)
	if err != nil {
		fmt.Fprintln(os.Stderr, err)
		os.Exit(1)
	}
	common.SetLogLevel(lv)
	common.Info("boot", "MiniNet starting with log=%s", *logLv)

	fmt.Println("MiniNet v0.2 (type `help` for commands, `quit` to exit)")
	stack := newStack()
	defer func() {
		if stack.l2 != nil {
			stack.l2.Close()
		}
		if stack.peerL2 != nil {
			stack.peerL2.Close()
		}
	}()

	reader := bufio.NewReader(os.Stdin)
	for {
		fmt.Print(">>> ")
		line, err := reader.ReadString('\n')
		if err != nil {
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
		case "peer":
			handlePeer(stack, parts[1:])
		case "arp":
			handleArp(stack, parts[1:])
		case "ping":
			handlePing(stack, parts[1:])
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
	fmt.Println("  peer up [localIP] [peerIP]               启动一个对端 goroutine (默认 10.0.0.1/10.0.0.2)")
	fmt.Println("  arp whois <ip>                           主动 ARP 问询")
	fmt.Println("  arp -a                                   列出 ARP 表")
	fmt.Println("  ping <ip> [count]                        发 N 个 ICMP echo (默认 4)")
	fmt.Println("  help / quit / exit")
}

// -------- route 命令 (阶段 ②) --------

func handleRoute(s *Stack, args []string) {
	if len(args) == 0 {
		fmt.Println("usage: route add <cidr> via <nh> [dev <iface>] | show | lookup <ip>")
		return
	}
	switch args[0] {
	case "add":
		if len(args) < 4 || args[2] != "via" {
			fmt.Println("usage: route add <cidr> via <nh> [dev <iface>]")
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

// -------- peer 命令 (阶段 ④) --------

func handlePeer(s *Stack, args []string) {
	if len(args) == 0 || args[0] != "up" {
		fmt.Println("usage: peer up [localIP] [peerIP]")
		return
	}
	if s.l2 != nil {
		fmt.Println("(error) peer already up; restart mnet to change")
		return
	}
	localIPStr, peerIPStr := "10.0.0.1", "10.0.0.2"
	if len(args) >= 2 {
		localIPStr = args[1]
	}
	if len(args) >= 3 {
		peerIPStr = args[2]
	}
	localIP, err := netpkg.ParseIPv4(localIPStr)
	if err != nil {
		fmt.Println("(error) local ip:", err)
		return
	}
	peerIP, err := netpkg.ParseIPv4(peerIPStr)
	if err != nil {
		fmt.Println("(error) peer ip:", err)
		return
	}

	// Loopback pair + 两个 L2 + 两个 L3
	drvA, drvB := link.LoopbackPair(
		link.MustParseMAC("aa:bb:cc:dd:ee:01"),
		link.MustParseMAC("aa:bb:cc:dd:ee:02"),
	)
	s.localMAC = drvA.MAC()
	s.localIP = localIP
	s.l2 = link.NewL2Layer(drvA, localIP)
	s.l3 = netpkg.NewL3Layer(s.l2, s.Routes)
	// 把 L2/L3 挂到 Stack
	s.L2 = s.l2
	s.L3 = s.l3
	// 对端 peer:独立的 Stack(RouteTable 本地的,ping 对面不会用)
	peerRoutes := netpkg.NewRouteTable()
	s.peerL2 = link.NewL2Layer(drvB, peerIP)
	s.peerL3 = netpkg.NewL3Layer(s.peerL2, peerRoutes)

	fmt.Printf("peer up: local %s (%s), peer %s (%s)\n",
		localIP, drvA.MAC(), peerIP, drvB.MAC())
}

// -------- arp 命令 (阶段 ④) --------

func handleArp(s *Stack, args []string) {
	if s.l2 == nil {
		fmt.Println("(error) peer not up; run `peer up` first")
		return
	}
	if len(args) == 0 {
		fmt.Println("usage: arp whois <ip> | arp -a")
		return
	}
	switch args[0] {
	case "whois":
		if len(args) < 2 {
			fmt.Println("usage: arp whois <ip>")
			return
		}
		ip, err := netpkg.ParseIPv4(args[1])
		if err != nil {
			fmt.Println("(error)", err)
			return
		}
		mac, err := s.l2.Resolve(ip)
		if err != nil {
			fmt.Printf("(error) %v\n", err)
			return
		}
		fmt.Printf("%s is-at %s\n", ip, mac)
	case "-a":
		ents := s.l2.ARP().Entries()
		if len(ents) == 0 {
			fmt.Println("(empty ARP table)")
			return
		}
		fmt.Println("IP               MAC")
		for ip, mac := range ents {
			fmt.Printf("%-16s %s\n", ip, mac)
		}
	default:
		fmt.Println("unknown arp subcommand:", args[0])
	}
}

// -------- ping 命令 (阶段 ⑤) --------

func handlePing(s *Stack, args []string) {
	if s.l3 == nil {
		fmt.Println("(error) peer not up; run `peer up` first")
		return
	}
	if len(args) == 0 {
		fmt.Println("usage: ping <ip> [count]")
		return
	}
	dst, err := netpkg.ParseIPv4(args[0])
	if err != nil {
		fmt.Println("(error)", err)
		return
	}
	count := 4
	if len(args) >= 2 {
		if n, err := strconv.Atoi(args[1]); err == nil && n > 0 {
			count = n
		}
	}

	fmt.Printf("PING %s: %d packets\n", dst, count)
	id := uint16(time.Now().Unix() & 0xFFFF)
	var ok, fail int
	for seq := 1; seq <= count; seq++ {
		r := s.l3.Ping(dst, id, uint16(seq), 2*time.Second)
		fmt.Println(r.String())
		if r.Timeout || r.Err != nil {
			fail++
		} else {
			ok++
		}
		if seq < count {
			time.Sleep(200 * time.Millisecond)
		}
	}
	fmt.Printf("--- %s ping statistics ---\n", dst)
	fmt.Printf("%d transmitted, %d received, %.0f%% packet loss\n",
		count, ok, float64(fail)*100.0/float64(count))
}
