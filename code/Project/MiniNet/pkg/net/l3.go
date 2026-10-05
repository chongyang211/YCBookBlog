// pkg/net/l3.go - L3 层封装 (IPv4 收发 + ICMP 处理 + ping 入口)
//
// 第 2 次会话 Step 5.2-5.3
//
// 职责:
//   1. 向 L2 注册 IPv4 handler,收到帧时解 IPv4 → 按 Proto 分发
//   2. 自动响应 ICMP echo-request (让对端 ping 得通)
//   3. 对外暴露 Ping(dstIP) 发起主动 ping
//
// L2 边界:
//   - L2 不懂 IPv4,只负责"按 nextHopIP → ARP → 发以太帧"
//   - L3 负责路由决策,把 nextHopIP 告诉 L2
package net

import (
	"errors"
	"fmt"
	"sync"
	"time"

	"mininet/pkg/common"
)

// L2Sender L3 对 L2 的依赖抽象 (避免 pkg/net 反向依赖 pkg/link)
// L2Layer 结构体通过方法签名一致自动实现
type L2Sender interface {
	// IP 本地主机 IP
	IP() IPv4Addr
	// SendIPv4Pkt L2 内部解 ARP(nextHop) 后发出以太帧
	SendIPv4Pkt(nextHop IPv4Addr, payload []byte) error
	// OnIPv4 L3 向 L2 注册 "收到 IPv4 payload 时叫我"
	OnIPv4(h func(payload []byte))
}

// PingResult 一次 ping 的结果
type PingResult struct {
	Seq     int
	TTL     uint8
	RTT     time.Duration
	Timeout bool
	Err     error
}

func (r PingResult) String() string {
	if r.Timeout {
		return fmt.Sprintf("Request timeout for icmp_seq=%d", r.Seq)
	}
	if r.Err != nil {
		return fmt.Sprintf("Error icmp_seq=%d: %v", r.Seq, r.Err)
	}
	return fmt.Sprintf("64 bytes: icmp_seq=%d ttl=%d time=%.3f ms",
		r.Seq, r.TTL, float64(r.RTT.Microseconds())/1000.0)
}

// TCPHandler L4 (TCP) 注册的回调:收到 proto=TCP 的 IPv4 包时调
type TCPHandler func(srcIP IPv4Addr, payload []byte)

// L3Layer IPv4 + ICMP 层
type L3Layer struct {
	l2     L2Sender
	routes *RouteTable

	// 等待 ping reply 的 waiter: (id<<16 | seq) → chan pingReply
	waitersMu sync.Mutex
	waiters   map[uint32]chan pingReply

	// L4 回调 (阶段 ⑥ 新增)
	tcpHandler TCPHandler
}

type pingReply struct {
	ttl uint8
	at  time.Time
}

func NewL3Layer(l2 L2Sender, routes *RouteTable) *L3Layer {
	l := &L3Layer{
		l2:      l2,
		routes:  routes,
		waiters: make(map[uint32]chan pingReply),
	}
	// 注册到 L2
	l2.OnIPv4(l.onIPv4)
	return l
}

func (l *L3Layer) Name() string { return "L3-Net" }

func (l *L3Layer) Dump() string {
	return fmt.Sprintf("ip=%s", l.l2.IP())
}

// -------- 收到 IPv4 帧的回调 --------

func (l *L3Layer) onIPv4(payload []byte) {
	h, body, err := DecodeIPv4(payload)
	if err != nil {
		common.Warn("ipv4", "decode: %v", err)
		return
	}
	// 不是给我的 → 不转发 (阶段 ⑤ 不做转发,只做端系统)
	if h.Dst != l.l2.IP() {
		common.Trace("ipv4", "drop pkt not for me: dst=%s mine=%s",
			h.Dst, l.l2.IP())
		return
	}
	common.Trace("ipv4", "RX src=%s proto=%s ttl=%d len=%d",
		h.Src, h.Proto, h.TTL, h.TotalLen)

	switch h.Proto {
	case ProtoICMP:
		l.handleICMP(h, body)
	case ProtoTCP:
		if l.tcpHandler != nil {
			l.tcpHandler(h.Src, body)
		} else {
			common.Trace("ipv4", "TCP seg but no handler, drop")
		}
	default:
		common.Trace("ipv4", "unsupported proto %s", h.Proto)
	}
}

// -------- 对 L4 的 API --------

// IP 本地 IP (实现 transport.L3Sender)
func (l *L3Layer) IP() IPv4Addr { return l.l2.IP() }

// SendIP L4 发包的入口 (阶段 ⑥ 新增)
func (l *L3Layer) SendIP(dst IPv4Addr, proto IPProto, payload []byte) error {
	return l.sendIPv4To(dst, proto, payload)
}

// OnTCP L4 注册 TCP handler
func (l *L3Layer) OnTCP(h func(srcIP IPv4Addr, payload []byte)) {
	l.tcpHandler = TCPHandler(h)
}

func (l *L3Layer) handleICMP(ipH *Ipv4Header, body []byte) {
	p, err := DecodeICMPEcho(body)
	if err != nil {
		common.Warn("icmp", "decode: %v", err)
		return
	}
	switch p.Type {
	case ICMPTypeEchoRequest:
		// 自动回 echo-reply
		common.Info("icmp", "RX echo-req from %s id=%d seq=%d → reply",
			ipH.Src, p.ID, p.Seq)
		rep := &ICMPEcho{
			Type: ICMPTypeEchoReply,
			ID:   p.ID,
			Seq:  p.Seq,
			Data: p.Data,
		}
		if err := l.sendIPv4To(ipH.Src, ProtoICMP, rep.Encode()); err != nil {
			common.Warn("icmp", "send reply: %v", err)
		}
	case ICMPTypeEchoReply:
		common.Trace("icmp", "RX echo-rep id=%d seq=%d", p.ID, p.Seq)
		key := uint32(p.ID)<<16 | uint32(p.Seq)
		l.waitersMu.Lock()
		ch, ok := l.waiters[key]
		if ok {
			delete(l.waiters, key)
		}
		l.waitersMu.Unlock()
		if ok {
			select {
			case ch <- pingReply{ttl: ipH.TTL, at: time.Now()}:
			default:
			}
		}
	}
}

// -------- 主动发送 --------

// sendIPv4To 封装 IPv4 头 + 路由决策 + 交给 L2
func (l *L3Layer) sendIPv4To(dst IPv4Addr, proto IPProto, payload []byte) error {
	// 1. 路由决策:默认目的 IP 即为 nextHop (直连);
	//    有路由表项时按 nextHop 走;nextHop 为 0 视为直连
	nextHop := dst
	if route, ok := l.routes.Lookup(dst); ok {
		if uint32(route.NextHop) != 0 && route.NextHop != dst {
			nextHop = route.NextHop
		}
	}
	// 2. 封装 IPv4 头
	h := NewIPv4Header(l.l2.IP(), dst, proto, len(payload))
	hdrBuf := h.Encode()
	full := append(hdrBuf, payload...)
	// 3. 交给 L2 (L2 内部解 ARP + 发以太帧)
	return l.l2.SendIPv4Pkt(nextHop, full)
}

// Ping 主动 ping 一次,阻塞至多 timeout
func (l *L3Layer) Ping(dst IPv4Addr, id, seq uint16, timeout time.Duration) PingResult {
	key := uint32(id)<<16 | uint32(seq)
	ch := make(chan pingReply, 1)
	l.waitersMu.Lock()
	l.waiters[key] = ch
	l.waitersMu.Unlock()

	// 发 ICMP echo-req
	req := &ICMPEcho{Type: ICMPTypeEchoRequest, ID: id, Seq: seq}
	t0 := time.Now()
	if err := l.sendIPv4To(dst, ProtoICMP, req.Encode()); err != nil {
		l.waitersMu.Lock()
		delete(l.waiters, key)
		l.waitersMu.Unlock()
		return PingResult{Seq: int(seq), Err: err}
	}

	// 等 reply
	select {
	case rep := <-ch:
		return PingResult{
			Seq: int(seq),
			TTL: rep.ttl,
			RTT: rep.at.Sub(t0),
		}
	case <-time.After(timeout):
		l.waitersMu.Lock()
		delete(l.waiters, key)
		l.waitersMu.Unlock()
		return PingResult{Seq: int(seq), Timeout: true}
	}
}

// ErrNoRoute 没有可用路由
var ErrNoRoute = errors.New("no route to host")
