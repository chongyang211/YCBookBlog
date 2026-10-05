// pkg/transport/bug3_test.go - 🔥 BUG-3 丢包重传现场
//
// 第 3 次会话 阶段 ⑦ Step 7.3 - 7.4
//
// 五步法:
//   1. 用 txHook 模拟 ~30% 丢包
//   2. 发 10 个数据段,看是否全部到达
//   3. 如果 RTO 实现正确,重传会救场
//   4. 如果关掉 RTO (本测试跳过),会卡死
//   5. 对比 "无丢包" 和 "有丢包+重传" 的到达率
package transport_test

import (
	"bytes"
	"io"
	"sync/atomic"
	"testing"
	"time"

	netpkg "mininet/pkg/net"
	_ "mininet/pkg/transport" // 共用 buildPair (在 e2e_test.go 里)
)

// TestLossRecovery 丢前 3 个数据段,靠 RTO 重传后能最终送达
func TestLossRecovery(t *testing.T) {
	l4A, l4B, cleanup := buildPair(t)
	defer cleanup()

	// 策略: A 侧前 3 个 "有 payload 的段" 全部丢弃 (握手/ACK/FIN 不丢)
	// 这样强制触发 RTO 重传路径
	var dropped, total atomic.Int64
	l4A.SetTxHook(func(seg []byte) bool {
		if len(seg) < 20 {
			return true
		}
		dataOff := int(seg[12]>>4) * 4
		if dataOff < 20 || dataOff > len(seg) {
			return true
		}
		hasPayload := len(seg) > dataOff
		if hasPayload {
			n := total.Add(1)
			if n <= 3 { // 丢前 3 个数据段
				dropped.Add(1)
				return false
			}
		}
		return true
	})

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

	ipB, _ := netpkg.ParseIPv4("10.0.0.2")
	time.Sleep(20 * time.Millisecond)
	ca, err := l4A.Dial(ipB, 9000, 3*time.Second)
	if err != nil {
		t.Fatal(err)
	}

	// 发 3 KB (3 个 MSS 段);前 3 个段都会被丢,靠重传救
	payload := bytes.Repeat([]byte("MINININET-TCP-WORKS!"), 150) // 3000 字节
	if _, err := ca.Write(payload); err != nil {
		t.Fatal(err)
	}
	ca.Close()

	select {
	case got := <-recvAll:
		if !bytes.Equal(got, payload) {
			t.Errorf("recv mismatch: len got=%d want=%d", len(got), len(payload))
		} else {
			t.Logf("✅ recovered %d bytes after %d drops (total data segs %d)",
				len(got), dropped.Load(), total.Load())
		}
	case <-time.After(15 * time.Second):
		t.Fatalf("timeout; dropped=%d total=%d", dropped.Load(), total.Load())
	}
}

// TestNoLossBaseline 无丢包基准
func TestNoLossBaseline(t *testing.T) {
	l4A, l4B, cleanup := buildPair(t)
	defer cleanup()

	ln, _ := l4B.Listen(9001)
	go func() {
		c, err := ln.Accept()
		if err != nil {
			return
		}
		io.Copy(io.Discard, c)
	}()
	ipB, _ := netpkg.ParseIPv4("10.0.0.2")
	time.Sleep(10 * time.Millisecond)
	ca, err := l4A.Dial(ipB, 9001, 2*time.Second)
	if err != nil {
		t.Fatal(err)
	}
	t0 := time.Now()
	ca.Write(bytes.Repeat([]byte{0xCC}, 10000))
	ca.Close()
	t.Logf("baseline: 10 KB no-loss took %v", time.Since(t0))
}
