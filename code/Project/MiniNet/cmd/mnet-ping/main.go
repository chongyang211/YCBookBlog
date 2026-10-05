// cmd/mnet-ping/main.go - 独立的 ping 诊断工具 (阶段 ⑤ Step 5.3)
//
// 用法:
//
//	./bin/mnet-ping                       使用默认 (local=10.0.0.1 peer=10.0.0.2)
//	./bin/mnet-ping 10.0.0.2              显式指定目的
//	./bin/mnet-ping -c 10 10.0.0.2        ping 10 次
//	./bin/mnet-ping -c 4 -local 192.168.1.1 -peer 192.168.1.2 192.168.1.2
//
// 本工具用 loopback 模式启动一个"目标 peer" + 一个"客户端栈",
// 从客户端栈 ping 目的 IP。全程零权限、零 sudo。
package main

import (
	"flag"
	"fmt"
	"os"
	"time"

	"mininet/pkg/common"
	"mininet/pkg/link"
	netpkg "mininet/pkg/net"
)

func main() {
	count := flag.Int("c", 4, "number of pings")
	logLv := flag.String("log", "info", "log level")
	localStr := flag.String("local", "10.0.0.1", "local IP")
	peerStr := flag.String("peer", "10.0.0.2", "peer IP (will host target)")
	flag.Usage = func() {
		fmt.Fprintln(os.Stderr, "usage: mnet-ping [flags] [dst_ip]")
		fmt.Fprintln(os.Stderr, "   default dst_ip = peer IP")
		flag.PrintDefaults()
	}
	flag.Parse()

	lv, _ := common.ParseLogLevel(*logLv)
	common.SetLogLevel(lv)

	localIP, err := netpkg.ParseIPv4(*localStr)
	if err != nil {
		die("local: %v", err)
	}
	peerIP, err := netpkg.ParseIPv4(*peerStr)
	if err != nil {
		die("peer: %v", err)
	}
	dstIP := peerIP
	if flag.NArg() >= 1 {
		dstIP, err = netpkg.ParseIPv4(flag.Arg(0))
		if err != nil {
			die("dst: %v", err)
		}
	}

	// 搭 loopback pair
	drvA, drvB := link.LoopbackPair(
		link.MustParseMAC("aa:bb:cc:dd:ee:01"),
		link.MustParseMAC("aa:bb:cc:dd:ee:02"),
	)
	l2Local := link.NewL2Layer(drvA, localIP)
	l2Peer := link.NewL2Layer(drvB, peerIP)
	defer l2Local.Close()
	defer l2Peer.Close()

	l3Local := netpkg.NewL3Layer(l2Local, netpkg.NewRouteTable())
	_ = netpkg.NewL3Layer(l2Peer, netpkg.NewRouteTable()) // peer 自动回 ICMP

	fmt.Printf("PING %s from %s: %d packets\n", dstIP, localIP, *count)
	id := uint16(time.Now().Unix() & 0xFFFF)
	var ok int
	var totalRTT time.Duration
	var minRTT, maxRTT time.Duration

	for seq := 1; seq <= *count; seq++ {
		r := l3Local.Ping(dstIP, id, uint16(seq), 2*time.Second)
		fmt.Println(r.String())
		if !r.Timeout && r.Err == nil {
			ok++
			totalRTT += r.RTT
			if minRTT == 0 || r.RTT < minRTT {
				minRTT = r.RTT
			}
			if r.RTT > maxRTT {
				maxRTT = r.RTT
			}
		}
		if seq < *count {
			time.Sleep(200 * time.Millisecond)
		}
	}

	loss := float64(*count-ok) * 100.0 / float64(*count)
	fmt.Printf("\n--- %s ping statistics ---\n", dstIP)
	fmt.Printf("%d packets transmitted, %d received, %.0f%% packet loss\n",
		*count, ok, loss)
	if ok > 0 {
		avg := totalRTT / time.Duration(ok)
		fmt.Printf("round-trip min/avg/max = %.3f/%.3f/%.3f ms\n",
			ms(minRTT), ms(avg), ms(maxRTT))
	}

	if ok == 0 {
		os.Exit(1)
	}
}

func ms(d time.Duration) float64 {
	return float64(d.Microseconds()) / 1000.0
}

func die(f string, a ...any) {
	fmt.Fprintf(os.Stderr, "mnet-ping: "+f+"\n", a...)
	os.Exit(2)
}
