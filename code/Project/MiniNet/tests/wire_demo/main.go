// tests/wire_demo/main.go - 🔥 BUG-1 现场:对比小端 BUG 版 vs 大端修复版
//
// 跑法:
//
//	go run ./tests/wire_demo
//	# 或
//	make wire-demo
//
// 预期输出 (关键是第 2-3 字节):
//
//	BUG 版       : 14 00  ← 小端颠倒,接收方当成 5120
//	修复版       : 00 14  ← 大端正确,接收方当成 20
package main

import (
	"encoding/binary"
	"fmt"

	"mininet/pkg/common"
)

// Ipv4Header 用 BeU16/BeU32 封装,字节序安全
// (阶段 ⑤ 会把这个结构体正式搬到 pkg/net/ipv4.go)
type Ipv4Header struct {
	VerIHL   byte         // offset 0
	TOS      byte         // offset 1
	TotalLen common.BeU16 // offset 2-3    ← 自知大端
	ID       common.BeU16 // offset 4-5
	FlagFrag common.BeU16 // offset 6-7
	TTL      byte         // offset 8
	Proto    byte         // offset 9
	Checksum common.BeU16 // offset 10-11
	SrcIP    common.BeU32 // offset 12-15  ← 自知大端
	DstIP    common.BeU32 // offset 16-19
}

func main() {
	fmt.Println("========== ❌ BUG 版 (TotalLen 用小端打包) ==========")
	showBuggy()
	fmt.Println()
	fmt.Println("========== ✅ 修复版 (用 BeU16/BeU32 类型封装) ==========")
	showFixed()
	fmt.Println()
	fmt.Println("========== 📊 结论 ==========")
	fmt.Println("修复的本质不是 '记得写 BigEndian' (人总会忘),")
	fmt.Println("而是 '让写错字节序在编译期就不被允许'。")
	fmt.Println("BeU16.Set 没有小端版可选 → bug 被类型系统关进牢笼。")
}

func showBuggy() {
	buf := make([]byte, 20)
	buf[0] = 0x45 // Version(4) + IHL(5)=20 字节
	buf[1] = 0x00 // TOS

	// ❌❌❌ BUG:用小端打包 TotalLen ❌❌❌
	// 网络协议规定 TotalLen 字段是大端 (Network Byte Order)
	// 但我们在 Go 里 "不小心" 用了 LittleEndian:
	binary.LittleEndian.PutUint16(buf[2:4], 20)

	buf[4], buf[5] = 0x00, 0x01 // Identification
	buf[6], buf[7] = 0x40, 0x00 // Flags + Fragment Offset
	buf[8] = 64                 // TTL
	buf[9] = 1                  // Protocol = ICMP
	// buf[10:12] checksum 暂时 0
	buf[12], buf[13], buf[14], buf[15] = 10, 0, 0, 1 // src 10.0.0.1
	buf[16], buf[17], buf[18], buf[19] = 10, 0, 0, 2 // dst 10.0.0.2

	fmt.Print(common.Hexdump(buf))
	wrong := binary.BigEndian.Uint16(buf[2:4])
	fmt.Printf("接收方按大端解析 TotalLen = %d (0x%04X)  ← 应该是 20!\n", wrong, wrong)
	fmt.Println("—— 接收方会试图读 5120 字节的 payload,一定崩")
}

func showFixed() {
	var h Ipv4Header
	h.VerIHL = 0x45
	h.TotalLen.Set(20) // 不管底层机器大小端,Set 永远写大端
	h.ID.Set(0x0001)
	h.FlagFrag.Set(0x4000)
	h.TTL = 64
	h.Proto = 1
	h.SrcIP.Set(0x0A000001) // 10.0.0.1
	h.DstIP.Set(0x0A000002) // 10.0.0.2

	// 把 struct 按字段挨个写进 buf (教学清晰)
	buf := make([]byte, 20)
	buf[0] = h.VerIHL
	buf[1] = h.TOS
	copy(buf[2:4], h.TotalLen[:])
	copy(buf[4:6], h.ID[:])
	copy(buf[6:8], h.FlagFrag[:])
	buf[8] = h.TTL
	buf[9] = h.Proto
	copy(buf[10:12], h.Checksum[:])
	copy(buf[12:16], h.SrcIP[:])
	copy(buf[16:20], h.DstIP[:])

	// 顺便算一次校验和填回去 (RFC1071)
	cs := common.InternetChecksum(buf)
	buf[10] = byte(cs >> 8)
	buf[11] = byte(cs)

	fmt.Print(common.Hexdump(buf))
	correct := binary.BigEndian.Uint16(buf[2:4])
	fmt.Printf("接收方按大端解析 TotalLen = %d (0x%04X)  ← 正确!\n", correct, correct)
	fmt.Printf("Checksum (RFC1071)      = 0x%04X\n", cs)

	// 自检:把 checksum 填回去再算一次,结果必须是 0
	if again := common.InternetChecksum(buf); again != 0 {
		fmt.Printf("⚠️ checksum self-check failed: 0x%04X\n", again)
	} else {
		fmt.Println("Checksum self-check     = 0  ✅")
	}
}
