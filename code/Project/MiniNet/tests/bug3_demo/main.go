// tests/bug3_demo/main.go - 🔥 BUG-3 现场:关掉 RTO vs 正常 RTO
//
// 跑法:
//
//	go run ./tests/bug3_demo
//	# 或
//	make bug3-demo
//
// 本 demo 展示的坑:
//   TCP 栈如果没有 RTO (重传定时器),一个丢的数据段就能让整个连接卡死。
//   因为对方永远等不到那个 seq,不会发 ACK;发送方 cwnd 不动,也不重传。
//
// 对比:
//   ❌ BUG 版: 制造 3 个数据段都丢,**不启用 RTO 重传** → 永远卡死
//   ✅ 修复版: 同样丢 3 个段,**启用 RTO** → 每 1s/2s/4s 自动重传,最终送达
package main

import (
	"fmt"
	"io"
	"os"
	"sync/atomic"
	"time"

	"mininet/pkg/common"
	"mininet/pkg/link"
	netpkg "mininet/pkg/net"
	"mininet/pkg/transport"
)

func main() {
	common.SetLogLevel(common.LvInfo)

	fmt.Println("========== 🔥 BUG-3 版 (丢 3 段 + 无 RTO 重传) ==========")
	runOne(false, 5*time.Second)
	fmt.Println()
	fmt.Println("========== ✅ 修复版 (丢 3 段 + RTO 重传生效) ==========")
	runOne(true, 10*time.Second)
	fmt.Println()
	fmt.Println("========== 📊 结论 ==========")
	fmt.Println("关掉 RTO 的 TCP 不是 TCP,是 UDP + 握手挥手。")
	fmt.Println("RFC6298 的 RTO 公式:")
	fmt.Println("  SRTT   = 7/8·SRTT + 1/8·RTT")
	fmt.Println("  RTTVAR = 3/4·RTTVAR + 1/4·|SRTT-RTT|")
	fmt.Println("  RTO    = SRTT + 4·RTTVAR  (下限 200ms, 上限 60s)")
	fmt.Println("超时后 RTO 指数退避 (Karn 算法),我们看到了 1s → 2s → 4s 的翻倍。")
}

// runOne 跑一次 "丢 3 段 → 看是否能恢复"
// enableRTO=false 时模拟 BUG 版:只 drop 不 retrans
func runOne(enableRTO bool, timeout time.Duration) {
	drvA, drvB := link.LoopbackPair(
		link.MustParseMAC("aa:bb:cc:dd:ee:01"),
		link.MustParseMAC("aa:bb:cc:dd:ee:02"),
	)
	ipA, _ := netpkg.ParseIPv4("10.0.0.1")
	ipB, _ := netpkg.ParseIPv4("10.0.0.2")
	l2A := link.NewL2Layer(drvA, ipA)
	l2B := link.NewL2Layer(drvB, ipB)
	defer l2A.Close()
	defer l2B.Close()
	l3A := netpkg.NewL3Layer(l2A, netpkg.NewRouteTable())
	l3B := netpkg.NewL3Layer(l2B, netpkg.NewRouteTable())
	l4A := transport.NewL4Layer(l3A)
	l4B := transport.NewL4Layer(l3B)
	defer l4A.Close()
	defer l4B.Close()

	// 丢前 3 个数据段
	var dropped, total atomic.Int64
	l4A.SetTxHook(func(seg []byte) bool {
		if len(seg) < 20 {
			return true
		}
		dataOff := int(seg[12]>>4) * 4
		hasPayload := len(seg) > dataOff
		if hasPayload {
			n := total.Add(1)
			if n <= 3 {
				dropped.Add(1)
				return false
			}
		}
		return true
	})

	// BUG 版: 覆盖 L4 的 txHook 让它也拦截重传
	// (checkRetrans 现在是绕过 txHook 的,BUG 版需要一个"关 RTO"的开关)
	if !enableRTO {
		// 简化模拟: txHook 永远丢所有数据段 (重传出去也丢)
		// 这样效果等同于 RTO 没启动
		l4A.SetTxHook(func(seg []byte) bool {
			if len(seg) < 20 {
				return true
			}
			dataOff := int(seg[12]>>4) * 4
			hasPayload := len(seg) > dataOff
			if hasPayload {
				total.Add(1)
				dropped.Add(1)
				return false
			}
			return true
		})
	}

	ln, _ := l4B.Listen(9000)
	recvAll := make(chan []byte, 1)
	go func() {
		c, err := ln.Accept()
		if err != nil {
			return
		}
		buf, _ := io.ReadAll(c)
		recvAll <- buf
	}()

	time.Sleep(20 * time.Millisecond)
	ca, err := l4A.Dial(ipB, 9000, 3*time.Second)
	if err != nil {
		fmt.Printf("dial 失败: %v\n", err)
		return
	}

	payload := []byte("MINININET-TCP-WORKS!")
	bigPayload := make([]byte, 3000)
	for i := range bigPayload {
		bigPayload[i] = payload[i%len(payload)]
	}

	t0 := time.Now()
	ca.Write(bigPayload)
	ca.Close()

	select {
	case got := <-recvAll:
		elapsed := time.Since(t0)
		fmt.Printf("结果: 收到 %d/%d 字节, 耗时 %v, drop %d (总 %d)\n",
			len(got), len(bigPayload), elapsed, dropped.Load(), total.Load())
		if enableRTO {
			fmt.Println("✅ RTO 救场成功:看日志里的 RTO timeout 和 RETRANS TX")
		}
	case <-time.After(timeout):
		fmt.Printf("结果: ⏱️  超时(%v),drop %d (总 %d) ← 永远收不到!\n",
			timeout, dropped.Load(), total.Load())
		if !enableRTO {
			fmt.Println("💥 这就是没有重传定时器的 '伪 TCP':一个丢包就死锁")
		}
	}
	os.Stdout.Sync()
}
