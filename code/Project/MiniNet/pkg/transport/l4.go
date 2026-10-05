// pkg/transport/l4.go - L4 层封装:TCP 协议栈 + 对外 API
//
// 第 3 次会话 Step 6.2-6.4 的核心
//
// 职责:
//   1. 向 L3 注册回调,收到 IPv4 (proto=6) 时解 TCP 头 → 按五元组路由到 TCB
//   2. 对外 API:
//        Listen(port)                 → 返回 Listener (支持 Accept)
//        Dial(remoteIP, remotePort)   → 返回 Conn (ESTABLISHED 后)
//   3. 内部:
//        每条 TCB 一个 "控制 goroutine" 跑状态机
//        收到的 segment 通过 chan 投递给对应 TCB
//
// 简化(与真实 Linux TCP 的差异):
//   - 不实现 PAWS/Window Scale/SACK/Timestamps
//   - MSL 教学值 1s (真实 30s)
//   - 发送端一个段最大 MSS(1024)
//   - 重传定时器留到 §⑦ 实现 (本波先用 "RTO=0 永不重传" 的 BUG 版)
package transport

import (
	"context"
	"errors"
	"fmt"
	"math/rand"
	"sync"
	"time"

	"mininet/pkg/common"
	netpkg "mininet/pkg/net"
)

// MSS 教学值 1024 字节 (真实 1460 或 536)
const MSS = 1024

// 教学 MSL (真实 Linux 60s,TIME_WAIT=2MSL=120s;这里缩短到 1s)
const MSL = 500 * time.Millisecond

var (
	ErrClosed         = errors.New("connection closed")
	ErrNotEstablished = errors.New("not established")
	ErrPortInUse      = errors.New("port already in use")
)

// L3Sender L4 对 L3 的依赖
// (pkg/net.L3Layer 通过方法签名一致自动实现)
type L3Sender interface {
	IP() netpkg.IPv4Addr
	// SendIPv4Pkt(dstIP, proto, payload) → L3 封 IPv4 头 + 路由 + L2 发
	// 返回 nil 表示发送成功 (不代表对端收到)
	SendIP(dst netpkg.IPv4Addr, proto netpkg.IPProto, payload []byte) error
	// 向 L3 注册 "收到 proto=TCP 的 IPv4 包时叫我"
	OnTCP(h func(srcIP netpkg.IPv4Addr, payload []byte))
}

// L4Layer TCP 栈
type L4Layer struct {
	l3 L3Sender

	mu sync.Mutex
	// 已建立的 TCB:按五元组查
	tcbs map[FourTuple]*TCB
	// listener:按 (localPort) 查
	listeners map[uint16]*Listener

	// 用于分配 ephemeral 端口
	nextPort uint16

	// 全局控制
	ctx    context.Context
	cancel context.CancelFunc

	// 测试钩子: 发送前拦截 (阶段 ⑦ BUG-3 用来模拟丢包)
	txHook func(seg []byte) bool // 返回 false 则丢弃
}

func NewL4Layer(l3 L3Sender) *L4Layer {
	ctx, cancel := context.WithCancel(context.Background())
	l := &L4Layer{
		l3:        l3,
		tcbs:      make(map[FourTuple]*TCB),
		listeners: make(map[uint16]*Listener),
		nextPort:  32768,
		ctx:       ctx,
		cancel:    cancel,
	}
	l3.OnTCP(l.onSegment)
	l.startRetransTimer() // 阶段 ⑦ Step 7.4: 后台 RTO 扫描
	return l
}

func (l *L4Layer) Name() string { return "L4-Transport" }

func (l *L4Layer) Dump() string {
	l.mu.Lock()
	defer l.mu.Unlock()
	return fmt.Sprintf("tcbs=%d listeners=%d", len(l.tcbs), len(l.listeners))
}

func (l *L4Layer) Close() { l.cancel() }

// SetTxHook 阶段 ⑦ 用:在发送前过滤
func (l *L4Layer) SetTxHook(h func(seg []byte) bool) {
	l.mu.Lock()
	l.txHook = h
	l.mu.Unlock()
}

// -------- 收到 TCP 段的分发 --------

func (l *L4Layer) onSegment(srcIP netpkg.IPv4Addr, payload []byte) {
	dstIP := l.l3.IP()
	h, data, err := DecodeTCP(srcIP, dstIP, payload)
	if err != nil {
		common.Warn("tcp", "decode: %v", err)
		return
	}
	common.Trace("tcp", "RX %s:%d → :%d flags=%s seq=%d ack=%d len=%d",
		srcIP, h.SrcPort, h.DstPort, h.Flags, h.Seq, h.Ack, len(data))

	key := FourTuple{
		LocalPort:  h.DstPort,
		RemotePort: h.SrcPort,
		LocalIP:    uint32(dstIP),
		RemoteIP:   uint32(srcIP),
	}

	l.mu.Lock()
	tcb, ok := l.tcbs[key]
	// 没有现成 TCB → 看是否有 listener (被动打开)
	if !ok {
		if ln, hasLn := l.listeners[h.DstPort]; hasLn && h.Flags.Has(FlagSYN) {
			// 新建一条 TCB,放入 pending
			tcb = newTCB(key)
			tcb.State = StateListen // 走 LISTEN → SYN_RCVD 分支
			l.tcbs[key] = tcb
			l.mu.Unlock()
			go l.runTCB(tcb, ln) // 新开状态机 goroutine
			// 把这个首包交给它处理
			l.deliverToTCB(tcb, h, data, srcIP)
			return
		}
		l.mu.Unlock()
		// 不属于任何连接 → 回 RST (简化:只在有 SYN 时才发)
		if h.Flags.Has(FlagSYN) {
			l.sendRST(srcIP, h.DstPort, h.SrcPort, 0, h.Seq.Add(1))
		}
		return
	}
	l.mu.Unlock()
	l.deliverToTCB(tcb, h, data, srcIP)
}

// deliverToTCB 把 segment 交给对应 TCB 处理
// 简化: 直接在当前 goroutine 处理(持 TCB 锁)
func (l *L4Layer) deliverToTCB(tcb *TCB, h *TCPHeader, data []byte, srcIP netpkg.IPv4Addr) {
	tcb.mu.Lock()
	defer tcb.mu.Unlock()
	l.handleSegment(tcb, h, data, srcIP)
}

// -------- 发送辅助 --------

// sendSegment 从 TCB 发出一个 segment
// 调用者可能持有 TCB 锁 (不要在这里等响应)
//
// 阶段 ⑦ 增量:
//   - 带数据/SYN/FIN 的段加入 rtq,RTO 超时会重传
//   - 纯 ACK 不入 rtq
func (l *L4Layer) sendSegment(tcb *TCB, flags TCPFlags, data []byte) error {
	remoteIP := netpkg.IPv4Addr(tcb.Tuple.RemoteIP)
	h := &TCPHeader{
		SrcPort: tcb.Tuple.LocalPort,
		DstPort: tcb.Tuple.RemotePort,
		Seq:     tcb.SndNXT,
		Flags:   flags,
		Window:  uint16(tcb.RcvWND),
	}
	if flags.Has(FlagACK) {
		h.Ack = tcb.RcvNXT
	}
	buf := h.Encode(l.l3.IP(), remoteIP, data)

	// 计算这个段占用多少 seqnum (SYN+1, FIN+1, data)
	segLen := uint32(len(data))
	if flags.Has(FlagSYN) {
		segLen++
	}
	if flags.Has(FlagFIN) {
		segLen++
	}

	// 阶段 ⑦: 占 seqnum 的段入重传队列 (纯 ACK 不入)
	if segLen > 0 {
		tcb.rtq.Add(&Segment{
			Seq:    h.Seq,
			Len:    segLen,
			Flags:  flags,
			Data:   append([]byte(nil), data...),
			SentAt: time.Now(),
		})
	}

	// 测试钩子: BUG-3 用这里模拟丢包
	l.mu.Lock()
	hook := l.txHook
	l.mu.Unlock()
	if hook != nil && !hook(buf) {
		common.Warn("tcp", "TX DROPPED by txHook: %s seq=%d len=%d",
			flags, h.Seq, len(data))
		// SndNXT 推进要跟真实发送一样 (对方以为我们发了,等重传)
		l.advanceSndNxt(tcb, flags, data)
		return nil
	}

	common.Trace("tcp", "TX :%d → %s:%d flags=%s seq=%d ack=%d len=%d",
		tcb.Tuple.LocalPort, remoteIP, tcb.Tuple.RemotePort,
		flags, h.Seq, h.Ack, len(data))

	if err := l.l3.SendIP(remoteIP, netpkg.ProtoTCP, buf); err != nil {
		return err
	}
	l.advanceSndNxt(tcb, flags, data)
	return nil
}

// advanceSndNxt 根据 flags 和 data 推进 SndNXT
func (l *L4Layer) advanceSndNxt(tcb *TCB, flags TCPFlags, data []byte) {
	// SYN 和 FIN 各占 1 个 seqnum
	if flags.Has(FlagSYN) {
		tcb.SndNXT = tcb.SndNXT.Add(1)
	}
	tcb.SndNXT = tcb.SndNXT.Add(uint32(len(data)))
	if flags.Has(FlagFIN) {
		tcb.SndNXT = tcb.SndNXT.Add(1)
	}
}

// sendRST 发裸 RST (不基于 TCB,收到不认识的 SYN 时用)
func (l *L4Layer) sendRST(dst netpkg.IPv4Addr, srcPort, dstPort uint16, seq, ack TCPSeq) {
	h := &TCPHeader{
		SrcPort: srcPort, DstPort: dstPort,
		Seq: seq, Ack: ack,
		Flags: FlagRST | FlagACK,
	}
	buf := h.Encode(l.l3.IP(), dst, nil)
	l.l3.SendIP(dst, netpkg.ProtoTCP, buf)
}

// -------- API: Dial / Listen --------

// Dial 主动打开到 remoteIP:remotePort
// 阻塞直到 ESTABLISHED 或超时
func (l *L4Layer) Dial(remoteIP netpkg.IPv4Addr, remotePort uint16, timeout time.Duration) (*Conn, error) {
	l.mu.Lock()
	localPort := l.nextPort
	l.nextPort++
	tuple := FourTuple{
		LocalPort:  localPort,
		RemotePort: remotePort,
		LocalIP:    uint32(l.l3.IP()),
		RemoteIP:   uint32(remoteIP),
	}
	if _, exists := l.tcbs[tuple]; exists {
		l.mu.Unlock()
		return nil, fmt.Errorf("tuple already exists")
	}
	tcb := newTCB(tuple)
	l.tcbs[tuple] = tcb
	l.mu.Unlock()

	// 启动状态机 goroutine
	go l.runTCB(tcb, nil)

	// 触发 ActiveOpen: 发 SYN
	tcb.mu.Lock()
	tcb.ISS = TCPSeq(rand.Uint32())
	tcb.SndNXT = tcb.ISS
	tcb.SndUNA = tcb.ISS
	tcb.setState(StateSynSent)
	common.Info("tcp", "state %s connecting to %s:%d",
		tcb.State, remoteIP, remotePort)
	l.sendSegment(tcb, FlagSYN, nil)
	tcb.mu.Unlock()

	// 等 ESTABLISHED
	select {
	case <-tcb.connected:
		if tcb.connectErr != nil {
			return nil, tcb.connectErr
		}
		return &Conn{tcb: tcb, l4: l}, nil
	case <-time.After(timeout):
		l.abortTCB(tcb)
		return nil, fmt.Errorf("dial timeout")
	}
}

// Listener 监听器
type Listener struct {
	port   uint16
	l4     *L4Layer
	accept chan *Conn // 新连接通过 chan 投递
	closed chan struct{}
	once   sync.Once
}

// Listen 开始监听 port
func (l *L4Layer) Listen(port uint16) (*Listener, error) {
	l.mu.Lock()
	defer l.mu.Unlock()
	if _, exists := l.listeners[port]; exists {
		return nil, ErrPortInUse
	}
	ln := &Listener{
		port:   port,
		l4:     l,
		accept: make(chan *Conn, 32),
		closed: make(chan struct{}),
	}
	l.listeners[port] = ln
	common.Info("tcp", "listening on :%d", port)
	return ln, nil
}

// Accept 阻塞等新连接
func (ln *Listener) Accept() (*Conn, error) {
	select {
	case c := <-ln.accept:
		return c, nil
	case <-ln.closed:
		return nil, ErrClosed
	}
}

func (ln *Listener) Close() error {
	ln.once.Do(func() {
		ln.l4.mu.Lock()
		delete(ln.l4.listeners, ln.port)
		ln.l4.mu.Unlock()
		close(ln.closed)
	})
	return nil
}

func (ln *Listener) Port() uint16 { return ln.port }

// -------- 内部: TCB 的生命周期 --------

// runTCB 每条 TCB 一个控制 goroutine (本波只处理 close 清理;状态机在 handleSegment 内同步跑)
func (l *L4Layer) runTCB(tcb *TCB, ln *Listener) {
	_ = ln
	<-tcb.closedCh
	// 清理
	l.mu.Lock()
	delete(l.tcbs, tcb.Tuple)
	l.mu.Unlock()
	common.Info("tcp", "tcb cleaned: %s", tcb.State)
}

// abortTCB 强制关闭 (dial 超时等)
func (l *L4Layer) abortTCB(tcb *TCB) {
	tcb.mu.Lock()
	if tcb.State != StateClosed {
		tcb.setState(StateClosed)
		select {
		case <-tcb.closedCh:
		default:
			close(tcb.closedCh)
		}
	}
	tcb.mu.Unlock()
}

// enterTimeWait 进入 TIME_WAIT 状态,2*MSL 后彻底关闭
func (l *L4Layer) enterTimeWait(tcb *TCB) {
	tcb.setState(StateTimeWait)
	common.Info("tcp", "state → TIME_WAIT, 2MSL=%v", 2*MSL)
	go func() {
		time.Sleep(2 * MSL)
		tcb.mu.Lock()
		tcb.setState(StateClosed)
		select {
		case <-tcb.closedCh:
		default:
			close(tcb.closedCh)
		}
		tcb.mu.Unlock()
	}()
}
