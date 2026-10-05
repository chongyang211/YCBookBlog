// pkg/transport/tcb.go - TCP 控制块 (Transmission Control Block)
//
// 第 3 次会话 Step 6.2-6.4
//
// TCB 是一条 TCP 连接的所有状态的集合体。RFC793 定义的字段:
//   - 五元组 (local/remote IP + Port + proto)
//   - 状态机当前状态
//   - 发送端:  SND.UNA  SND.NXT  SND.WND  ISS
//   - 接收端:  RCV.NXT  RCV.WND  IRS
//
// 本案例的简化 TCB 保留核心字段;阶段 ⑦ 的滑窗/拥塞会再加 cwnd/ssthresh/RTO 等。
package transport

import (
	"sync"
	"time"
)

// TCPEvent 状态机的输入事件 (给单测/日志用)
type TCPEvent int

const (
	EvActiveOpen  TCPEvent = iota // 应用层 connect()
	EvPassiveOpen                 // 应用层 listen()
	EvClose                       // 应用层 close()
	EvRxSyn                       // 收到 SYN
	EvRxAck                       // 收到 ACK
	EvRxSynAck                    // 收到 SYN+ACK
	EvRxFin                       // 收到 FIN
	EvRxData                      // 收到数据
	EvRxRst                       // 收到 RST
	EvTimeout                     // 定时器
)

var eventNames = [...]string{
	"ActiveOpen", "PassiveOpen", "Close",
	"RxSYN", "RxACK", "RxSYN+ACK", "RxFIN", "RxData", "RxRST", "Timeout",
}

func (e TCPEvent) String() string {
	if int(e) >= 0 && int(e) < len(eventNames) {
		return eventNames[e]
	}
	return "?"
}

// FourTuple 五元组的 4-tuple 部分 (proto 固定 TCP)
type FourTuple struct {
	LocalPort  uint16
	RemotePort uint16
	LocalIP    uint32 // 用 uint32 做 map key
	RemoteIP   uint32
}

// TCB TCP 控制块
type TCB struct {
	mu sync.Mutex

	// 五元组
	Tuple FourTuple

	// 当前状态
	State TCPState

	// -------- 发送端 --------
	ISS     TCPSeq    // Initial Send Sequence
	SndUNA  TCPSeq    // 已发送但未 ACK 的最小 seq
	SndNXT  TCPSeq    // 下一个要发的 seq
	SndWND  uint32    // 发送窗口大小
	LastAck time.Time // 最后收到 ACK 的时间 (计算 RTO 用)

	// -------- 接收端 --------
	IRS    TCPSeq // Initial Receive Sequence
	RcvNXT TCPSeq // 期望下一个收到的 seq
	RcvWND uint32 // 接收窗口大小

	// -------- 应用层缓冲 --------
	// 发送缓冲: 应用层 Write 塞进来,状态机线程拉去发
	// 接收缓冲: 状态机线程塞进来,应用层 Read 拉去
	sendBuf []byte
	recvBuf []byte
	// 用 chan 通知"有事情发生"
	readable chan struct{}
	writable chan struct{} // 空间可用时唤醒 Write

	// -------- 关闭信号 --------
	// 到达 ESTABLISHED 时 close(connected);握手失败也 close
	connected  chan struct{}
	connectErr error
	// 对端 FIN 后到达 CLOSE_WAIT 时置 true
	peerClosed bool
	// 完全关闭 (CLOSED)
	closedCh chan struct{}

	// -------- 阶段 ⑦ 新增:拥塞控制 + RTO + 重传队列 --------
	cong *CongCtrl
	rto  *RTOCalc
	rtq  *RetransQueue
}

func newTCB(t FourTuple) *TCB {
	return &TCB{
		Tuple:     t,
		State:     StateClosed,
		SndWND:    65535,
		RcvWND:    65535,
		readable:  make(chan struct{}, 1),
		writable:  make(chan struct{}, 1),
		connected: make(chan struct{}),
		closedCh:  make(chan struct{}),
		cong:      NewCongCtrl(),
		rto:       NewRTOCalc(),
		rtq:       NewRetransQueue(),
	}
}

// notifyReadable 非阻塞唤醒等待读的 goroutine
func (t *TCB) notifyReadable() {
	select {
	case t.readable <- struct{}{}:
	default:
	}
}

func (t *TCB) notifyWritable() {
	select {
	case t.writable <- struct{}{}:
	default:
	}
}

// setState 原子切换状态并打日志
// 调用者必须持有 t.mu
func (t *TCB) setState(ns TCPState) {
	// log 由外层统一打,避免在锁内 IO
	t.State = ns
}
