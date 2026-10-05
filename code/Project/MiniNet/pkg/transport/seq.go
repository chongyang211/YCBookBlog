// pkg/transport/seq.go - TCP 序列号的 32 位环绕算术
//
// 第 3 次会话 Step 6.1 (辅助)
//
// 为什么需要专门的类型?
//   TCP seqnum 是 uint32,会环绕(wrap-around)。
//   直接用 "seqA > seqB" 比较会出错:
//     seqA = 0xFFFFFFF0 (环绕前)
//     seqB = 0x00000010 (环绕后)
//   数值上 seqA > seqB,但 TCP 语义上 seqB 更晚。
//
// 正确做法: 用 int32(seqA - seqB) 判断,靠溢出得到有符号的差值。
//   (seqA - seqB) 若 int32 < 0 → seqA 在 seqB 之前
//   (seqA - seqB) 若 int32 > 0 → seqA 在 seqB 之后
//
// RFC793 的原话:
//   "A < B" means (B - A) & 2^31 == 0 and (B - A) != 0
package transport

// TCPSeq TCP 序列号
type TCPSeq uint32

// Add 环绕加法
func (s TCPSeq) Add(n uint32) TCPSeq { return TCPSeq(uint32(s) + n) }

// Sub 环绕减法,返回 (s - other) 的无符号距离
func (s TCPSeq) Sub(other TCPSeq) uint32 { return uint32(s) - uint32(other) }

// LT s < other (正确处理环绕)
func (s TCPSeq) LT(other TCPSeq) bool { return int32(uint32(s)-uint32(other)) < 0 }

// LE s <= other
func (s TCPSeq) LE(other TCPSeq) bool { return int32(uint32(s)-uint32(other)) <= 0 }

// GT s > other
func (s TCPSeq) GT(other TCPSeq) bool { return int32(uint32(s)-uint32(other)) > 0 }

// GE s >= other
func (s TCPSeq) GE(other TCPSeq) bool { return int32(uint32(s)-uint32(other)) >= 0 }

// Between 判断 s 是否在 (lo, hi] 区间内(左开右闭)
// 用于 "ACK 是否覆盖了某个已发送的 seg"
func (s TCPSeq) Between(lo, hi TCPSeq) bool {
	return s.GT(lo) && s.LE(hi)
}
