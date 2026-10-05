// pkg/link/l2.go - L2 层封装 (Driver + ARP + IPv4 分发器)
//
// 第 2 次会话 Step 4.3 的"胶水"
//
// 职责:
//   1. 启动一个 goroutine 从 Driver 不停读帧
//   2. 按 EtherType 分发:
//        0x0806 ARP   → 内部处理 (回应 who-has / 更新 ArpTable)
//        0x0800 IPv4  → 回调 L3 层注册的 handler
//   3. 对上层暴露 Resolve(ip) 和 SendIPv4(dstMAC, payload)
//
// 这是整个协议栈的"L2 → L3" 分发点。阶段 ⑥ TCP 栈接入时也走这条路。
package link

import (
	"context"
	"errors"
	"fmt"

	"mininet/pkg/common"
	netpkg "mininet/pkg/net"
)

// IPv4Handler L3 层注册的回调,收到一个 IPv4 帧时被调用
// payload 是 IPv4 包的字节(不含以太头)
// (L3 不关心 srcMAC,从 payload 的 IPv4 头里拿 srcIP 更合适)
type IPv4Handler func(payload []byte)

// L2Layer 实现 Layer 接口 (Name/Dump),也是 L3/ARP 的操纵入口
// 并实现 pkg/net.L2Sender 接口 (方法签名一致,无需显式声明)
type L2Layer struct {
	drv Driver
	ip  netpkg.IPv4Addr // 本机 IP (阶段 ⑤ 用)
	arp *ArpTable

	// L3 回调
	ipv4Handler IPv4Handler

	// 后台 goroutine 控制
	ctx    context.Context
	cancel context.CancelFunc
}

// NewL2Layer 构造;立刻启动后台 goroutine 监听 drv
func NewL2Layer(drv Driver, ip netpkg.IPv4Addr) *L2Layer {
	ctx, cancel := context.WithCancel(context.Background())
	l := &L2Layer{
		drv:    drv,
		ip:     ip,
		arp:    NewArpTable(),
		ctx:    ctx,
		cancel: cancel,
	}
	go l.rxLoop()
	return l
}

func (l *L2Layer) Name() string { return "L2-Link" }

func (l *L2Layer) Dump() string {
	ents := l.arp.Entries()
	return fmt.Sprintf("mac=%s ip=%s arp_entries=%d",
		l.drv.MAC(), l.ip, len(ents))
}

// Driver 返回底层驱动 (给测试/诊断用)
func (l *L2Layer) Driver() Driver { return l.drv }

// ARP 返回 ArpTable (给 REPL `arp -a` 用)
func (l *L2Layer) ARP() *ArpTable { return l.arp }

// IP 返回本机 IP
func (l *L2Layer) IP() netpkg.IPv4Addr { return l.ip }

// OnIPv4 L3 层注册回调,收到 IPv4 帧时触发
// 参数用匿名函数类型,与 pkg/net.L2Sender 接口签名完全一致
// (Go 的结构类型匹配不认 named type alias,只认签名字面量)
func (l *L2Layer) OnIPv4(h func(payload []byte)) { l.ipv4Handler = IPv4Handler(h) }

// Close 停止后台 goroutine 和 ARP GC
func (l *L2Layer) Close() {
	l.cancel()
	l.arp.Close()
	l.drv.Close()
}

// -------- 后台 rx 循环 --------

func (l *L2Layer) rxLoop() {
	for {
		if l.ctx.Err() != nil {
			return
		}
		buf, err := l.drv.Recv()
		if err != nil {
			if errors.Is(err, ErrDriverClosed) {
				return
			}
			common.Warn("l2", "recv error: %v", err)
			return
		}
		dst, src, et, payload, err := DecodeFrame(buf)
		if err != nil {
			common.Warn("l2", "decode frame: %v", err)
			continue
		}
		// 只接受发给自己或广播的帧
		if dst != l.drv.MAC() && !dst.IsBroadcast() {
			common.Trace("l2", "drop frame not for me: dst=%s", dst)
			continue
		}

		common.Trace("l2", "RX %s type=%s len=%d", src, et, len(payload))

		_ = src // src MAC 可用于诊断,目前只在 trace 打印
		switch et {
		case EtherTypeARP:
			l.handleARP(payload)
		case EtherTypeIPv4:
			if l.ipv4Handler != nil {
				l.ipv4Handler(payload)
			} else {
				common.Trace("l2", "no IPv4 handler, drop")
			}
		default:
			common.Trace("l2", "unknown ethertype: %s", et)
		}
	}
}

// -------- ARP 处理 --------

func (l *L2Layer) handleARP(payload []byte) {
	p, err := DecodeArp(payload)
	if err != nil {
		common.Warn("arp", "decode: %v", err)
		return
	}

	// 不管是 request 还是 reply,先把 sender 的映射存一下
	// (真实 Linux 也这么做,叫 "snooping")
	l.arp.Insert(p.Sender.IP, p.Sender.MAC)

	switch p.Op {
	case ArpOpRequest:
		// 问我 → 回答
		if p.Target.IP == l.ip {
			common.Info("arp", "who-has %s? reply %s is-at %s",
				p.Target.IP, l.ip, l.drv.MAC())
			reply := &ArpPacket{Op: ArpOpReply}
			reply.Sender.MAC = l.drv.MAC()
			reply.Sender.IP = l.ip
			reply.Target.MAC = p.Sender.MAC
			reply.Target.IP = p.Sender.IP
			frame := EncodeFrame(p.Sender.MAC, l.drv.MAC(),
				EtherTypeARP, reply.Encode())
			if err := l.drv.Send(frame); err != nil {
				common.Warn("arp", "send reply: %v", err)
			}
		}
	case ArpOpReply:
		common.Info("arp", "learn %s is-at %s", p.Sender.IP, p.Sender.MAC)
		// Insert 已经在上面做过
	}
}

// Resolve 查询(或广播请求) 某个 IP 的 MAC,阻塞最多 3s
// 1. 先查缓存
// 2. 没命中就广播 ARP who-has,阻塞等 reply
func (l *L2Layer) Resolve(ip netpkg.IPv4Addr) (MAC, error) {
	if mac, ok := l.arp.Lookup(ip); ok {
		return mac, nil
	}

	// 注册 waiter(先注册再发包,避免丢回复)
	ch := l.arp.WaitFor(ip)

	// 构造并广播 ARP request
	req := &ArpPacket{Op: ArpOpRequest}
	req.Sender.MAC = l.drv.MAC()
	req.Sender.IP = l.ip
	req.Target.IP = ip // target MAC 全 0
	frame := EncodeFrame(Broadcast, l.drv.MAC(), EtherTypeARP, req.Encode())
	if err := l.drv.Send(frame); err != nil {
		return MAC{}, err
	}
	common.Info("arp", "who-has %s ? (broadcast)", ip)

	// 等 reply
	select {
	case mac := <-ch:
		return mac, nil
	case <-l.ctx.Done():
		return MAC{}, ErrDriverClosed
	case <-afterARPTimeout():
		return MAC{}, fmt.Errorf("arp timeout for %s", ip)
	}
}

// SendIPv4 组装以太帧并发送 IPv4 payload (已知目的 MAC 时直接用)
func (l *L2Layer) SendIPv4(dstMAC MAC, payload []byte) error {
	frame := EncodeFrame(dstMAC, l.drv.MAC(), EtherTypeIPv4, payload)
	common.Trace("l2", "TX %s type=IPv4 len=%d", dstMAC, len(payload))
	return l.drv.Send(frame)
}

// SendIPv4Pkt 实现 pkg/net.L2Sender 接口
// L3 只给 nextHopIP,L2 内部解 ARP + 发以太帧
func (l *L2Layer) SendIPv4Pkt(nextHop netpkg.IPv4Addr, payload []byte) error {
	mac, err := l.Resolve(nextHop)
	if err != nil {
		return err
	}
	return l.SendIPv4(mac, payload)
}

// 便于单测 mock 的间接层
func afterARPTimeout() <-chan struct{} {
	done := make(chan struct{})
	go func() {
		timer := newTimer(ArpResolveTimeout)
		<-timer
		close(done)
	}()
	return done
}
