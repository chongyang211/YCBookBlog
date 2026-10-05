// pkg/transport/retrans.go - 重传定时器 + RTO + 拥塞控制
//
// 第 3 次会话 阶段 ⑦ Step 7.2 - 7.4
//
// 核心概念:
//
//   SRTT (Smoothed RTT):    SRTT = α·SRTT + (1-α)·RTT       α=7/8
//   RTTVAR (RTT variation): RTTVAR = β·RTTVAR + (1-β)·|SRTT-RTT|  β=3/4
//   RTO = SRTT + 4·RTTVAR   (下限 200ms,上限 60s)
//
//   拥塞控制 (Reno):
//     cwnd 初始 = 1 MSS (慢启动)
//     收到 ACK: cwnd += MSS  (慢启动,指数增长)
//     cwnd >= ssthresh 后: cwnd += MSS*MSS/cwnd  (拥塞避免,线性增长)
//     RTO 超时: ssthresh = cwnd/2, cwnd = 1 MSS, 回到慢启动
//     3 个重复 ACK (快速重传): ssthresh = cwnd/2, cwnd = ssthresh+3MSS,
//                              重传丢的段 (不等 RTO)
package transport

import (
	"sync"
	"time"

	"mininet/pkg/common"
	netpkg "mininet/pkg/net"
)

// 常量
const (
	// RTO 下限/上限 (RFC6298)
	RTOMin = 200 * time.Millisecond
	RTOMax = 60 * time.Second
	// 初始 RTO (RFC6298: 1s)
	RTOInit = 1 * time.Second

	// cwnd 初始 (RFC5681: 2-4 MSS;教学用 1 MSS 以凸显慢启动)
	InitialCwnd = MSS
	// ssthresh 初始 (RFC5681: 任意大;教学用 64 KB)
	InitialSsthresh = 64 * 1024

	// 快速重传阈值
	DupAckThreshold = 3
)

// Segment 已发送但未 ACK 的段 (用于重传)
type Segment struct {
	Seq     TCPSeq
	Len     uint32 // 占的 seqnum 数 (含 SYN/FIN)
	Flags   TCPFlags
	Data    []byte
	SentAt  time.Time
	Retrans int // 已重传次数
}

// CongCtrl 拥塞控制状态
type CongCtrl struct {
	Cwnd     uint32 // 拥塞窗口 (字节)
	Ssthresh uint32 // 慢启动阈值
	DupAcks  int    // 连续重复 ACK 数
	LastAck  TCPSeq // 上次收到的 ACK (用来数 dup)
}

func NewCongCtrl() *CongCtrl {
	return &CongCtrl{
		Cwnd:     InitialCwnd,
		Ssthresh: InitialSsthresh,
	}
}

// OnAck 收到新 ACK 时调用,返回是否"推进"(非 dup)
func (c *CongCtrl) OnAck(ack TCPSeq, bytesAcked uint32) {
	if bytesAcked > 0 {
		// 新 ACK → 推进窗口
		c.DupAcks = 0
		if c.Cwnd < c.Ssthresh {
			// 慢启动:每 ACK +1 MSS
			c.Cwnd += MSS
		} else {
			// 拥塞避免:每 RTT +1 MSS (近似 MSS²/cwnd per ACK)
			inc := uint32(MSS) * uint32(MSS) / c.Cwnd
			if inc < 1 {
				inc = 1
			}
			c.Cwnd += inc
		}
	} else if ack == c.LastAck {
		// 重复 ACK
		c.DupAcks++
	}
	c.LastAck = ack
}

// OnTimeout RTO 超时,拥塞处理
func (c *CongCtrl) OnTimeout() {
	c.Ssthresh = c.Cwnd / 2
	if c.Ssthresh < 2*MSS {
		c.Ssthresh = 2 * MSS
	}
	c.Cwnd = InitialCwnd
	c.DupAcks = 0
}

// OnFastRetrans 3 dup ACK 触发快速重传
func (c *CongCtrl) OnFastRetrans() {
	c.Ssthresh = c.Cwnd / 2
	if c.Ssthresh < 2*MSS {
		c.Ssthresh = 2 * MSS
	}
	c.Cwnd = c.Ssthresh + 3*MSS
}

// RTOCalc Jacobson/Karn 平滑算法
type RTOCalc struct {
	SRTT   time.Duration
	RTTVar time.Duration
	RTO    time.Duration
	first  bool
}

func NewRTOCalc() *RTOCalc {
	return &RTOCalc{RTO: RTOInit, first: true}
}

// Update 用一次新的 RTT 样本更新 RTO
func (r *RTOCalc) Update(rtt time.Duration) {
	if r.first {
		r.SRTT = rtt
		r.RTTVar = rtt / 2
		r.first = false
	} else {
		// |SRTT - RTT|
		diff := r.SRTT - rtt
		if diff < 0 {
			diff = -diff
		}
		r.RTTVar = (3*r.RTTVar + diff) / 4
		r.SRTT = (7*r.SRTT + rtt) / 8
	}
	r.RTO = r.SRTT + 4*r.RTTVar
	if r.RTO < RTOMin {
		r.RTO = RTOMin
	}
	if r.RTO > RTOMax {
		r.RTO = RTOMax
	}
}

// Backoff RTO 加倍 (Karn 算法:重传后不采样 RTT,只加倍 RTO)
func (r *RTOCalc) Backoff() {
	r.RTO *= 2
	if r.RTO > RTOMax {
		r.RTO = RTOMax
	}
}

// -------- RetransQueue 重传队列 (per-connection) --------

// RetransQueue 已发送未 ACK 的 segments
type RetransQueue struct {
	mu    sync.Mutex
	queue []*Segment
}

func NewRetransQueue() *RetransQueue {
	return &RetransQueue{}
}

// Add 加入一个已发送的 segment
func (q *RetransQueue) Add(s *Segment) {
	q.mu.Lock()
	defer q.mu.Unlock()
	q.queue = append(q.queue, s)
}

// RemoveAckedUpTo 移除所有 end <= ack 的 segment,返回总字节数
func (q *RetransQueue) RemoveAckedUpTo(ack TCPSeq) uint32 {
	q.mu.Lock()
	defer q.mu.Unlock()
	var acked uint32
	kept := q.queue[:0]
	for _, s := range q.queue {
		end := s.Seq.Add(s.Len)
		if end.LE(ack) {
			acked += s.Len
		} else {
			kept = append(kept, s)
		}
	}
	q.queue = kept
	return acked
}

// Oldest 返回最老的未 ACK 段 (nil 若队列空)
func (q *RetransQueue) Oldest() *Segment {
	q.mu.Lock()
	defer q.mu.Unlock()
	if len(q.queue) == 0 {
		return nil
	}
	return q.queue[0]
}

// Len 返回队列长度
func (q *RetransQueue) Len() int {
	q.mu.Lock()
	defer q.mu.Unlock()
	return len(q.queue)
}

// -------- 启动/停止 RTO 定时器 --------

// startRetransTimer 阶段 ⑦ Step 7.4: 启动 RTO 定时器
// 简化实现: 一个 goroutine 周期扫描所有 TCB
// (真实 Linux: 每 TCB 一个 min-heap,这里偷懒)
func (l *L4Layer) startRetransTimer() {
	go func() {
		tk := time.NewTicker(50 * time.Millisecond)
		defer tk.Stop()
		for {
			select {
			case <-l.ctx.Done():
				return
			case <-tk.C:
				l.checkRetrans()
			}
		}
	}()
}

func (l *L4Layer) checkRetrans() {
	l.mu.Lock()
	tcbs := make([]*TCB, 0, len(l.tcbs))
	for _, t := range l.tcbs {
		tcbs = append(tcbs, t)
	}
	l.mu.Unlock()

	now := time.Now()
	for _, tcb := range tcbs {
		tcb.mu.Lock()
		if tcb.rtq == nil || tcb.State == StateClosed {
			tcb.mu.Unlock()
			continue
		}
		oldest := tcb.rtq.Oldest()
		if oldest == nil {
			tcb.mu.Unlock()
			continue
		}
		if now.Sub(oldest.SentAt) < tcb.rto.RTO {
			tcb.mu.Unlock()
			continue
		}
		// 超时 → 重传 + RTO 加倍 + 拥塞回退
		oldest.Retrans++
		oldest.SentAt = now
		tcb.rto.Backoff()
		tcb.cong.OnTimeout()
		common.Warn("tcp", "RTO timeout, retrans seq=%d (try=%d), new RTO=%v cwnd=%d",
			oldest.Seq, oldest.Retrans, tcb.rto.RTO, tcb.cong.Cwnd)
		// 真实重传
		remoteIP := netpkg.IPv4Addr(tcb.Tuple.RemoteIP)
		h := &TCPHeader{
			SrcPort: tcb.Tuple.LocalPort, DstPort: tcb.Tuple.RemotePort,
			Seq: oldest.Seq, Ack: tcb.RcvNXT,
			Flags: oldest.Flags | FlagACK, Window: uint16(tcb.RcvWND),
		}
		buf := h.Encode(l.l3.IP(), remoteIP, oldest.Data)
		// 重传绕过 txHook (教学 BUG-3 是"原发丢",重传应该成功)
		common.Info("tcp", "RETRANS TX %s seq=%d len=%d",
			oldest.Flags, oldest.Seq, len(oldest.Data))
		l.l3.SendIP(remoteIP, netpkg.ProtoTCP, buf)
		if oldest.Retrans > 10 {
			common.Error("tcp", "too many retrans, abort")
			l.closeNow(tcb)
		}
		tcb.mu.Unlock()
	}
}
