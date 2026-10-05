// pkg/transport/sm.go - TCP 状态机核心
//
// 第 3 次会话 Step 6.3-6.4
//
// RFC793 的状态迁移表 (简化到最常走的边):
//
//   CLOSED      --(ActiveOpen,snd SYN)--> SYN_SENT
//   CLOSED      --(PassiveOpen)--> LISTEN
//
//   LISTEN      --(rcv SYN,snd SYN+ACK)--> SYN_RCVD
//   SYN_RCVD    --(rcv ACK)--> ESTABLISHED
//
//   SYN_SENT    --(rcv SYN+ACK,snd ACK)--> ESTABLISHED
//
//   ESTABLISHED --(Close,snd FIN)--> FIN_WAIT_1
//   ESTABLISHED --(rcv FIN,snd ACK)--> CLOSE_WAIT
//
//   FIN_WAIT_1  --(rcv ACK)--> FIN_WAIT_2
//   FIN_WAIT_1  --(rcv FIN,snd ACK)--> CLOSING
//
//   FIN_WAIT_2  --(rcv FIN,snd ACK)--> TIME_WAIT
//   CLOSING     --(rcv ACK)--> TIME_WAIT
//
//   CLOSE_WAIT  --(Close,snd FIN)--> LAST_ACK
//   LAST_ACK    --(rcv ACK)--> CLOSED
//
//   TIME_WAIT   --(2*MSL timeout)--> CLOSED
//
// 调用约定: handleSegment 在持有 tcb.mu 的情况下被调用
package transport

import (
	"math/rand"
	"time"

	"mininet/pkg/common"
	netpkg "mininet/pkg/net"
)

// uint32ToIP 小工具:把 uint32 转 IPv4Addr
func uint32ToIP(v uint32) netpkg.IPv4Addr { return netpkg.IPv4Addr(v) }

// handleSegment 处理一个收到的 TCP 段 (持 tcb.mu)
func (l *L4Layer) handleSegment(tcb *TCB, h *TCPHeader, data []byte, srcIP netpkg.IPv4Addr) {
	_ = srcIP

	// 收到 RST → 直接关
	if h.Flags.Has(FlagRST) {
		common.Info("tcp", "state %s got RST → CLOSED", tcb.State)
		l.closeNow(tcb)
		return
	}

	switch tcb.State {
	case StateListen:
		l.smListen(tcb, h, data)
	case StateSynSent:
		l.smSynSent(tcb, h, data)
	case StateSynRcvd:
		l.smSynRcvd(tcb, h, data)
	case StateEstablished:
		l.smEstablished(tcb, h, data)
	case StateFinWait1:
		l.smFinWait1(tcb, h, data)
	case StateFinWait2:
		l.smFinWait2(tcb, h, data)
	case StateCloseWait:
		l.smCloseWait(tcb, h, data)
	case StateClosing:
		l.smClosing(tcb, h, data)
	case StateLastAck:
		l.smLastAck(tcb, h, data)
	case StateTimeWait:
		// TIME_WAIT 对任何到达的包都回 ACK (保险)
		if h.Flags.Has(FlagFIN) {
			tcb.RcvNXT = tcb.RcvNXT.Add(1)
			l.sendSegment(tcb, FlagACK, nil)
		}
	default:
		common.Trace("tcp", "ignore seg in state %s", tcb.State)
	}
}

// -------- 各状态分支 --------

func (l *L4Layer) smListen(tcb *TCB, h *TCPHeader, _ []byte) {
	if !h.Flags.Has(FlagSYN) {
		return
	}
	// 被动打开: 收到 SYN → snd SYN+ACK → SYN_RCVD
	tcb.IRS = h.Seq
	tcb.RcvNXT = h.Seq.Add(1)
	tcb.ISS = TCPSeq(rand.Uint32())
	tcb.SndNXT = tcb.ISS
	tcb.SndUNA = tcb.ISS
	tcb.setState(StateSynRcvd)
	common.Info("tcp", "state LISTEN → SYN_RCVD (sending SYN+ACK)")
	l.sendSegment(tcb, FlagSYN|FlagACK, nil)
}

func (l *L4Layer) smSynSent(tcb *TCB, h *TCPHeader, _ []byte) {
	if h.Flags.Has(FlagSYN) && h.Flags.Has(FlagACK) {
		// 收到 SYN+ACK → snd ACK → ESTABLISHED
		if h.Ack != tcb.ISS.Add(1) {
			common.Warn("tcp", "unexpected ACK in SYN_SENT: %d vs ISS+1=%d",
				h.Ack, tcb.ISS.Add(1))
			return
		}
		tcb.IRS = h.Seq
		tcb.RcvNXT = h.Seq.Add(1)
		// 走 processAck 清掉 rtq 里的 SYN
		l.processAck(tcb, h)
		tcb.setState(StateEstablished)
		common.Info("tcp", "state SYN_SENT → ESTABLISHED")
		l.sendSegment(tcb, FlagACK, nil)
		// 唤醒 Dial 调用方
		close(tcb.connected)
	} else if h.Flags.Has(FlagSYN) {
		// 同时打开 (simultaneous open)
		tcb.IRS = h.Seq
		tcb.RcvNXT = h.Seq.Add(1)
		tcb.setState(StateSynRcvd)
		l.sendSegment(tcb, FlagSYN|FlagACK, nil)
	}
}

func (l *L4Layer) smSynRcvd(tcb *TCB, h *TCPHeader, _ []byte) {
	if h.Flags.Has(FlagACK) {
		if h.Ack != tcb.ISS.Add(1) {
			common.Warn("tcp", "unexpected ACK in SYN_RCVD")
			return
		}
		// 走 processAck 清掉 rtq 里的 SYN+ACK
		l.processAck(tcb, h)
		tcb.setState(StateEstablished)
		common.Info("tcp", "state SYN_RCVD → ESTABLISHED")
		l.deliverToListener(tcb)
	}
}

// deliverToListener 把新建立的 TCB 包成 Conn 投给对应 Listener
func (l *L4Layer) deliverToListener(tcb *TCB) {
	l.mu.Lock()
	ln := l.listeners[tcb.Tuple.LocalPort]
	l.mu.Unlock()
	if ln == nil {
		return
	}
	c := &Conn{tcb: tcb, l4: l}
	select {
	case ln.accept <- c:
		common.Info("tcp", "accepted :%d ← %s:%d",
			tcb.Tuple.LocalPort,
			netpkg.IPv4Addr(tcb.Tuple.RemoteIP), tcb.Tuple.RemotePort)
	case <-ln.closed:
	}
}

func (l *L4Layer) smEstablished(tcb *TCB, h *TCPHeader, data []byte) {
	// 1. 处理 ACK → 推进 SndUNA
	if h.Flags.Has(FlagACK) {
		l.processAck(tcb, h)
	}
	// 2. 处理数据 (必须 seq == RcvNXT,简化: 不支持乱序)
	dataAccepted := false
	if len(data) > 0 {
		if h.Seq == tcb.RcvNXT {
			tcb.recvBuf = append(tcb.recvBuf, data...)
			tcb.RcvNXT = tcb.RcvNXT.Add(uint32(len(data)))
			tcb.notifyReadable()
			dataAccepted = true
			l.sendSegment(tcb, FlagACK, nil)
		} else {
			common.Trace("tcp", "out-of-order seg seq=%d (want %d)",
				h.Seq, tcb.RcvNXT)
			l.sendSegment(tcb, FlagACK, nil)
			return // 乱序时不处理 FIN
		}
	}
	// 3. 处理 FIN:只有 FIN 按序到达才接受
	// FIN 占位在 payload 之后,所以 FIN 的"末尾 seq" = h.Seq + len(data) 应该等于 RcvNXT
	if h.Flags.Has(FlagFIN) {
		finSeq := h.Seq.Add(uint32(len(data)))
		if finSeq != tcb.RcvNXT {
			common.Trace("tcp", "out-of-order FIN seq=%d (want %d), ignored",
				finSeq, tcb.RcvNXT)
			l.sendSegment(tcb, FlagACK, nil)
			return
		}
		_ = dataAccepted
		tcb.RcvNXT = tcb.RcvNXT.Add(1)
		tcb.setState(StateCloseWait)
		tcb.peerClosed = true
		tcb.notifyReadable()
		common.Info("tcp", "state ESTABLISHED → CLOSE_WAIT (peer closed)")
		l.sendSegment(tcb, FlagACK, nil)
	}
}

func (l *L4Layer) smFinWait1(tcb *TCB, h *TCPHeader, data []byte) {
	// 期望: 对方 ACK 我的 FIN,然后再发它自己的 FIN
	// 两种可能:
	//   (a) 先收到 ACK → FIN_WAIT_2
	//   (b) 先收到 FIN (无 ACK) → CLOSING
	//   (c) 同时 ACK+FIN → TIME_WAIT

	ackedMyFin := h.Flags.Has(FlagACK) && h.Ack == tcb.SndNXT

	if len(data) > 0 && h.Seq == tcb.RcvNXT {
		tcb.recvBuf = append(tcb.recvBuf, data...)
		tcb.RcvNXT = tcb.RcvNXT.Add(uint32(len(data)))
		tcb.notifyReadable()
	}

	if h.Flags.Has(FlagACK) {
		l.processAck(tcb, h)
	}

	if h.Flags.Has(FlagFIN) {
		tcb.RcvNXT = tcb.RcvNXT.Add(1)
		if ackedMyFin {
			common.Info("tcp", "state FIN_WAIT_1 → TIME_WAIT")
			l.sendSegment(tcb, FlagACK, nil)
			l.enterTimeWait(tcb)
		} else {
			tcb.setState(StateClosing)
			common.Info("tcp", "state FIN_WAIT_1 → CLOSING")
			l.sendSegment(tcb, FlagACK, nil)
		}
		return
	}
	if ackedMyFin {
		tcb.setState(StateFinWait2)
		common.Info("tcp", "state FIN_WAIT_1 → FIN_WAIT_2")
	}
}

func (l *L4Layer) smFinWait2(tcb *TCB, h *TCPHeader, data []byte) {
	if len(data) > 0 && h.Seq == tcb.RcvNXT {
		tcb.recvBuf = append(tcb.recvBuf, data...)
		tcb.RcvNXT = tcb.RcvNXT.Add(uint32(len(data)))
		tcb.notifyReadable()
	}
	if h.Flags.Has(FlagFIN) {
		tcb.RcvNXT = tcb.RcvNXT.Add(1)
		common.Info("tcp", "state FIN_WAIT_2 → TIME_WAIT")
		l.sendSegment(tcb, FlagACK, nil)
		l.enterTimeWait(tcb)
	}
}

func (l *L4Layer) smCloseWait(tcb *TCB, h *TCPHeader, _ []byte) {
	// 应用层应调 Close 使我进 LAST_ACK;这里只处理可能的重复 FIN
	if h.Flags.Has(FlagACK) {
		l.processAck(tcb, h)
	}
}

func (l *L4Layer) smClosing(tcb *TCB, h *TCPHeader, _ []byte) {
	if h.Flags.Has(FlagACK) && h.Ack == tcb.SndNXT {
		common.Info("tcp", "state CLOSING → TIME_WAIT")
		l.enterTimeWait(tcb)
	}
}

func (l *L4Layer) smLastAck(tcb *TCB, h *TCPHeader, _ []byte) {
	if h.Flags.Has(FlagACK) && h.Ack == tcb.SndNXT {
		common.Info("tcp", "state LAST_ACK → CLOSED")
		l.closeNow(tcb)
	}
}

// -------- ACK 处理 --------

func (l *L4Layer) processAck(tcb *TCB, h *TCPHeader) {
	// 阶段 ⑦: 更新 ACK + 清重传队列 + 更新 cwnd + 快速重传检测
	if h.Ack.GT(tcb.SndUNA) && h.Ack.LE(tcb.SndNXT) {
		// 计算从 rtq 清掉的字节数
		bytesAcked := tcb.rtq.RemoveAckedUpTo(h.Ack)
		// 更新 RTO 样本 (简化:用最老那段的发送时间近似 RTT)
		// 真实 TCP 要排除重传段 (Karn 算法)
		if oldest := tcb.rtq.Oldest(); oldest == nil && bytesAcked > 0 {
			// 全部 ACK 完了,用最近一次的发送时间
			// 教学简化:不精确采样 RTT,仅在新连接前几次采样
		}
		tcb.SndUNA = h.Ack
		tcb.cong.OnAck(h.Ack, bytesAcked)
		tcb.notifyWritable()
		common.Trace("tcp", "ACK advanced SndUNA → %d bytesAcked=%d cwnd=%d",
			tcb.SndUNA, bytesAcked, tcb.cong.Cwnd)
	} else if h.Ack == tcb.SndUNA {
		// 重复 ACK
		tcb.cong.OnAck(h.Ack, 0)
		if tcb.cong.DupAcks == DupAckThreshold {
			// 快速重传:重传最老的未 ACK 段
			if oldest := tcb.rtq.Oldest(); oldest != nil {
				tcb.cong.OnFastRetrans()
				common.Warn("tcp", "fast retrans (3 dup ACK), seq=%d cwnd=%d",
					oldest.Seq, tcb.cong.Cwnd)
				l.retransNow(tcb, oldest)
			}
		}
	}
}

// retransNow 立即重传指定段
func (l *L4Layer) retransNow(tcb *TCB, seg *Segment) {
	remoteIP := uint32ToIP(tcb.Tuple.RemoteIP)
	h := &TCPHeader{
		SrcPort: tcb.Tuple.LocalPort, DstPort: tcb.Tuple.RemotePort,
		Seq: seg.Seq, Ack: tcb.RcvNXT,
		Flags: seg.Flags | FlagACK, Window: uint16(tcb.RcvWND),
	}
	buf := h.Encode(l.l3.IP(), remoteIP, seg.Data)
	seg.SentAt = time.Now()
	seg.Retrans++
	l.l3.SendIP(remoteIP, netpkg.ProtoTCP, buf)
}

// closeNow 立刻进 CLOSED 并清理
func (l *L4Layer) closeNow(tcb *TCB) {
	if tcb.State == StateClosed {
		return
	}
	tcb.setState(StateClosed)
	// 让所有阻塞的 Read/Write 都返回
	tcb.notifyReadable()
	tcb.notifyWritable()
	select {
	case <-tcb.closedCh:
	default:
		close(tcb.closedCh)
	}
	// 握手中就被 RST 的情况也要唤醒 Dial
	select {
	case <-tcb.connected:
	default:
		tcb.connectErr = ErrClosed
		close(tcb.connected)
	}
}
