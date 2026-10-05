// tests/bug2_demo/main.go - 🔥 BUG-2 现场:子网掩码/路由不一致
//
// 跑法:
//
//	go run ./tests/bug2_demo
//	# 或
//	make bug2-demo
//
// 本 demo 展示的坑:
//   本机 10.0.0.1/16  (以为子网是 /16),
//   同网段的 10.1.0.2 本应直连 ARP,但我们错配了一条路由:
//     route add 10.1.0.0/24 via 10.0.0.254
//   由于路由表最长前缀匹配,10.1.0.2 命中了 /24 的错误规则,
//   走到不存在的网关 10.0.0.254 → ARP 超时。
//
// 修复:删掉错误路由,就能走最长前缀 /16 的直连规则 (或根本不该加这条路由)。
package main

import (
	"fmt"
	"os"
	"time"

	"mininet/pkg/common"
	"mininet/pkg/link"
	netpkg "mininet/pkg/net"
)

func main() {
	common.SetLogLevel(common.LvInfo)

	// 本机 10.0.0.1,peer 10.1.0.2 (属于 /16 内, /24 外)
	localIP := mustIP("10.0.0.1")
	peerIP := mustIP("10.1.0.2")

	drvA, drvB := link.LoopbackPair(
		link.MustParseMAC("aa:bb:cc:dd:ee:01"),
		link.MustParseMAC("aa:bb:cc:dd:ee:02"),
	)
	l2L := link.NewL2Layer(drvA, localIP)
	l2P := link.NewL2Layer(drvB, peerIP)
	defer l2L.Close()
	defer l2P.Close()

	routes := netpkg.NewRouteTable()
	l3L := netpkg.NewL3Layer(l2L, routes)
	_ = netpkg.NewL3Layer(l2P, netpkg.NewRouteTable())

	fmt.Println("========== 🔥 BUG-2 版 (错误路由抢占) ==========")
	fmt.Println()
	// 错误配置:加了一条看似合理但抢占了直连的 /24 路由
	// nextHop 10.0.0.254 根本不存在
	routes.Add(netpkg.Route{
		Dst:     mustCIDR("10.1.0.0/24"),
		NextHop: mustIP("10.0.0.254"),
		Iface:   "wrong-gw",
	})
	// 加了默认路由作为备胎 (正常情况下 /24 优先命中)
	routes.Add(netpkg.Route{
		Dst:     mustCIDR("0.0.0.0/0"),
		NextHop: localIP,
		Iface:   "lo",
	})

	fmt.Println("当前路由表:")
	fmt.Print(routes.Show())
	fmt.Printf("\n尝试 ping %s (预期走错路由 → 卡在 ARP 10.0.0.254)...\n", peerIP)

	t0 := time.Now()
	r := l3L.Ping(peerIP, 1, 1, 5*time.Second)
	fmt.Printf("结果: %s  (耗时 %v)\n\n", r.String(), time.Since(t0))

	fmt.Println("========== ✅ 修复版 (清空错误路由) ==========")
	fmt.Println()
	// 修复:删掉错误的 /24,只保留默认路由
	// (RouteTable 没实现 Delete,这里直接重建)
	routes2 := netpkg.NewRouteTable()
	// 修复后的路由:
	//   把直连子网 /16 显式加上,让同网段流量走直连
	routes2.Add(netpkg.Route{
		Dst:     mustCIDR("10.0.0.0/16"),
		NextHop: mustIP("0.0.0.0"), // 0 表示直连
		Iface:   "lo",
	})
	// 把 L3 替换成新路由表
	l3L2 := netpkg.NewL3Layer(l2L, routes2)
	fmt.Println("修复后的路由表:")
	fmt.Print(routes2.Show())
	fmt.Printf("\n再 ping %s ...\n", peerIP)

	t0 = time.Now()
	r = l3L2.Ping(peerIP, 2, 1, 2*time.Second)
	fmt.Printf("结果: %s  (耗时 %v)\n\n", r.String(), time.Since(t0))

	fmt.Println("========== 📊 结论 ==========")
	fmt.Println("最长前缀匹配是双刃剑:")
	fmt.Println("  - 优点:'具体' 规则压过 '通用' 规则,符合 '最接近真相' 的直觉")
	fmt.Println("  - 风险:一条拼写错的 /24 能让整个本应直连的子网全跑网关")
	fmt.Println()
	fmt.Println("排查套路:")
	fmt.Println("  1. 用 >>> route show 对照 Linux `ip route show` 看规则")
	fmt.Println("  2. 用 >>> route lookup <ip> 看具体 IP 命中哪条")
	fmt.Println("  3. ARP 超时时第一反应: 看目标 IP 的最终 nextHop 是否可达")
}

func mustIP(s string) netpkg.IPv4Addr {
	ip, err := netpkg.ParseIPv4(s)
	if err != nil {
		fmt.Fprintln(os.Stderr, err)
		os.Exit(1)
	}
	return ip
}

func mustCIDR(s string) netpkg.Subnet {
	c, err := netpkg.ParseCIDR(s)
	if err != nil {
		fmt.Fprintln(os.Stderr, err)
		os.Exit(1)
	}
	return c
}
