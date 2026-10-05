// tests/bug7_demo/main.go - 🔥 BUG-7: TCP 队头阻塞 vs QUIC 多流
//
// 跑法: make bug7-demo
//
// 场景: 浏览器要并发下载 3 个资源 (CSS/JS/IMG)
//   - HTTP/1.1: 用 3 条独立 TCP 连接 (老方案,每条都要 3-way 握手)
//   - HTTP/2 over TCP: 1 条 TCP, 3 个 stream 复用
//     ⚠ 但 TCP 字节流有序 → 任何一个丢包,后面所有字节都卡住
//     → 一个 stream 的丢包让其他 stream 也等 (TCP 层队头阻塞)
//   - HTTP/3 over QUIC: 1 条 UDP, 3 个 stream 独立
//     ✅ stream 之间彼此独立, A 丢包不影响 B 和 C 的进度
//
// 本 demo 对比:
//   场景 A: 3 个 TCP stream 走同一个连接, 中间丢一个包 → 全部卡
//   场景 B: 3 个 QUIC stream 走同一个连接, 中间丢一个包 → 只这一个卡
//
// 教学简化: 我们用 Reader 包装模拟"丢包窗口"
package main

import (
	"bufio"
	"fmt"
	"io"
	"net"
	"sync"
	"time"

	"mininet/pkg/common"
	"mininet/pkg/quic"
)

const (
	chunkSize = 32
	numChunks = 10
)

// lossyReader 模拟 "第 N 次读失败一段时间"
type lossyReader struct {
	r        io.Reader
	cnt      int
	stallAt  int
	stallDur time.Duration
}

func (lr *lossyReader) Read(p []byte) (int, error) {
	lr.cnt++
	if lr.cnt == lr.stallAt {
		time.Sleep(lr.stallDur)
	}
	return lr.r.Read(p)
}

// =============== 场景 A: TCP 单连接 3 stream (模拟 HTTP/2) ===============
func scenarioTCP() {
	fmt.Println("----- 🔥 场景 A: TCP 单连接 3 流 (模拟 HTTP/2 的队头阻塞) -----")
	ln, _ := net.Listen("tcp", "127.0.0.1:0")
	defer ln.Close()
	go func() {
		c, err := ln.Accept()
		if err != nil {
			return
		}
		defer c.Close()
		// 把 3 个流交错写入同一个 TCP 字节流
		// 格式: "[sid]data...\n"
		for i := 0; i < numChunks; i++ {
			for sid := 0; sid < 3; sid++ {
				data := fmt.Sprintf("[s%d-%02d]", sid, i)
				c.Write([]byte(data))
				time.Sleep(5 * time.Millisecond)
			}
		}
	}()

	c, _ := net.Dial("tcp", ln.Addr().String())
	defer c.Close()
	// 丢包: 读 TCP 到第 5 次时, 卡 500ms (任何一个流都受影响)
	lr := &lossyReader{r: c, stallAt: 5, stallDur: 500 * time.Millisecond}

	// 3 个解复用器: 每读到一个 "[sN-xx]" 分发到对应 stream
	streamStart := make(map[int]time.Time)
	streamEnd := make(map[int]time.Time)
	for i := 0; i < 3; i++ {
		streamStart[i] = time.Now()
	}
	br := bufio.NewReader(lr)
	for i := 0; i < 3*numChunks; i++ {
		head := make([]byte, 7) // "[sN-NN]"
		if _, err := io.ReadFull(br, head); err != nil {
			break
		}
		sid := int(head[2] - '0')
		streamEnd[sid] = time.Now()
	}
	for sid := 0; sid < 3; sid++ {
		fmt.Printf("  Stream %d 完成耗时: %v\n", sid, streamEnd[sid].Sub(streamStart[sid]))
	}
}

// =============== 场景 B: QUIC 单连接 3 stream (每流独立) ===============
func scenarioQUIC() {
	fmt.Println()
	fmt.Println("----- ✅ 场景 B: QUIC 单连接 3 流 (每流独立) -----")
	ln, _ := quic.Listen("127.0.0.1:0")
	defer ln.Close()

	go func() {
		conn, err := ln.Accept()
		if err != nil {
			return
		}
		var wg sync.WaitGroup
		for i := 0; i < 3; i++ {
			s := conn.AcceptStream(5 * time.Second)
			if s == nil {
				return
			}
			wg.Add(1)
			go func(st *quic.Stream, sid int) {
				defer wg.Done()
				// 读客户端发的 "GO" 后开始发
				buf := make([]byte, 2)
				st.Read(buf)
				for i := 0; i < numChunks; i++ {
					data := fmt.Sprintf("[s%d-%02d]", sid, i)
					st.Write([]byte(data))
					time.Sleep(5 * time.Millisecond)
				}
				st.WriteFin(nil)
			}(s, i)
		}
		wg.Wait()
	}()

	c, err := quic.Dial(ln.Addr().String())
	if err != nil {
		fmt.Println("quic dial:", err)
		return
	}
	defer c.Close()
	time.Sleep(20 * time.Millisecond)

	// 开 3 个流,每个流独立计时;其中 stream 0 的接收会故意延迟 (模拟它的 UDP 包丢了)
	results := make([]time.Duration, 3)
	var wg sync.WaitGroup
	for i := 0; i < 3; i++ {
		wg.Add(1)
		go func(sid int) {
			defer wg.Done()
			s := c.OpenStream()
			defer s.Close()
			s.Write([]byte("GO"))
			t0 := time.Now()
			// stream 0 模拟丢包延迟
			if sid == 0 {
				time.Sleep(500 * time.Millisecond) // 延迟开始读
			}
			buf := make([]byte, 7)
			for i := 0; i < numChunks; i++ {
				if _, err := io.ReadFull(s, buf); err != nil {
					break
				}
			}
			results[sid] = time.Since(t0)
		}(i)
	}
	wg.Wait()
	for sid := 0; sid < 3; sid++ {
		fmt.Printf("  Stream %d 完成耗时: %v\n", sid, results[sid])
	}
}

func main() {
	common.SetLogLevel(common.LvWarn)
	fmt.Println("================================================================")
	fmt.Println("🔥 BUG-7: TCP 单连接多流 vs QUIC 多流")
	fmt.Println("   场景: 3 个资源并发下载, 其中一个在中途卡 500ms")
	fmt.Println("================================================================")

	scenarioTCP()
	scenarioQUIC()

	fmt.Println()
	fmt.Println("================================================================")
	fmt.Println("📊 结论:")
	fmt.Println("   TCP 单流:   因为字节流有序, 一个点卡住 → 所有流都等")
	fmt.Println("   QUIC 多流:  stream 之间独立, A 延迟不影响 B/C")
	fmt.Println("   → 真实 HTTP/3 场景: 弱网 (地铁/飞机) 下首屏加载快 30~50%")
	fmt.Println("================================================================")
}
