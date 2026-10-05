// pkg/link/arp.go - ARP 协议 + ArpTable 老化表
//
// 第 2 次会话 Step 4.3 + 4.4
//
// ARP (RFC826) 用于把 IPv4 地址解析成 MAC 地址。
//
// 工作流程:
//   1. A 想和同网段 10.0.0.2 通信,但不知道它的 MAC
//   2. A 广播 "who-has 10.0.0.2 tell 10.0.0.1" (ARP request)
//   3. 10.0.0.2 听到广播,单播回复 "10.0.0.2 is-at aa:bb:cc:..."  (ARP reply)
//   4. A 把映射存进 ArpTable,下次 60s 内直接用缓存
//
// ARP 帧结构 (28 字节,不含以太头):
//
//   +--------+--------+----+----+--------+---------+--------+---------+
//   |  HTYPE | PTYPE  |HLEN|PLEN| OPCODE | SHA(6)  | SPA(4) | THA(6)  | TPA(4)
//   | 2(0001)| 2(0800)| 1(6)| 1(4)| 2     |         |        |         |
//   +--------+--------+----+----+--------+---------+--------+---------+
//
//   HTYPE   硬件类型 (0x0001 = Ethernet)
//   PTYPE   协议类型 (0x0800 = IPv4)
//   HLEN    硬件长度 (6)
//   PLEN    协议长度 (4)
//   OPCODE  1 = request, 2 = reply
//   SHA     Sender Hardware Address
//   SPA     Sender Protocol Address (IP)
//   THA     Target Hardware Address  (请求时为 0)
//   TPA     Target Protocol Address  (IP)
package link

import (
	"context"
	"fmt"
	"sync"
	"time"

	netpkg "mininet/pkg/net"
)

// ARP opcode
type ArpOp uint16

const (
	ArpOpRequest ArpOp = 1
	ArpOpReply   ArpOp = 2
)

// ARP 包的二进制结构
type ArpPacket struct {
	Op     ArpOp
	Sender struct {
		MAC MAC
		IP  netpkg.IPv4Addr
	}
	Target struct {
		MAC MAC
		IP  netpkg.IPv4Addr
	}
}

// ArpPacketSize 28 字节
const ArpPacketSize = 28

// Encode 打包成 28 字节
func (p *ArpPacket) Encode() []byte {
	buf := make([]byte, ArpPacketSize)
	// HTYPE = 0x0001 (Ethernet)
	buf[0], buf[1] = 0x00, 0x01
	// PTYPE = 0x0800 (IPv4)
	buf[2], buf[3] = 0x08, 0x00
	// HLEN=6, PLEN=4
	buf[4], buf[5] = 6, 4
	// OPCODE (大端)
	buf[6] = byte(uint16(p.Op) >> 8)
	buf[7] = byte(p.Op)
	// SHA (6)
	copy(buf[8:14], p.Sender.MAC[:])
	// SPA (4) - IP 是 host-order uint32,我们按大端写
	spa := uint32(p.Sender.IP)
	buf[14], buf[15] = byte(spa>>24), byte(spa>>16)
	buf[16], buf[17] = byte(spa>>8), byte(spa)
	// THA (6)
	copy(buf[18:24], p.Target.MAC[:])
	// TPA (4)
	tpa := uint32(p.Target.IP)
	buf[24], buf[25] = byte(tpa>>24), byte(tpa>>16)
	buf[26], buf[27] = byte(tpa>>8), byte(tpa)
	return buf
}

// DecodeArp 解析 ARP 包
func DecodeArp(buf []byte) (*ArpPacket, error) {
	if len(buf) < ArpPacketSize {
		return nil, fmt.Errorf("arp packet too short: %d", len(buf))
	}
	// 校验 HTYPE / PTYPE / HLEN / PLEN (不严格,只认 Ethernet+IPv4)
	if buf[0] != 0 || buf[1] != 1 {
		return nil, fmt.Errorf("unsupported htype: %02x%02x", buf[0], buf[1])
	}
	if buf[2] != 0x08 || buf[3] != 0x00 {
		return nil, fmt.Errorf("unsupported ptype: %02x%02x", buf[2], buf[3])
	}
	p := &ArpPacket{
		Op: ArpOp(uint16(buf[6])<<8 | uint16(buf[7])),
	}
	copy(p.Sender.MAC[:], buf[8:14])
	p.Sender.IP = netpkg.IPv4Addr(uint32(buf[14])<<24 | uint32(buf[15])<<16 |
		uint32(buf[16])<<8 | uint32(buf[17]))
	copy(p.Target.MAC[:], buf[18:24])
	p.Target.IP = netpkg.IPv4Addr(uint32(buf[24])<<24 | uint32(buf[25])<<16 |
		uint32(buf[26])<<8 | uint32(buf[27]))
	return p, nil
}

// -------- ArpTable 带老化的 ARP 缓存 --------

const (
	// ArpTTL 一条表项的最大寿命
	ArpTTL = 60 * time.Second
	// ArpGCInterval 后台清理 goroutine 的扫描间隔
	ArpGCInterval = 10 * time.Second
	// ArpResolveTimeout 等待一次 ARP 回复的最长时间
	ArpResolveTimeout = 3 * time.Second
)

type arpEntry struct {
	MAC       MAC
	ExpiresAt time.Time
}

type ArpTable struct {
	mu      sync.RWMutex
	entries map[netpkg.IPv4Addr]arpEntry

	// 等待 ARP reply 的 waiter 通道 (ip → chan reply)
	waitersMu sync.Mutex
	waiters   map[netpkg.IPv4Addr][]chan MAC

	// 后台 GC
	cancel context.CancelFunc
}

func NewArpTable() *ArpTable {
	ctx, cancel := context.WithCancel(context.Background())
	t := &ArpTable{
		entries: make(map[netpkg.IPv4Addr]arpEntry),
		waiters: make(map[netpkg.IPv4Addr][]chan MAC),
		cancel:  cancel,
	}
	go t.gcLoop(ctx)
	return t
}

func (t *ArpTable) gcLoop(ctx context.Context) {
	tk := time.NewTicker(ArpGCInterval)
	defer tk.Stop()
	for {
		select {
		case <-ctx.Done():
			return
		case now := <-tk.C:
			t.mu.Lock()
			for ip, e := range t.entries {
				if now.After(e.ExpiresAt) {
					delete(t.entries, ip)
				}
			}
			t.mu.Unlock()
		}
	}
}

// Insert 新增/更新一条 ARP 映射 (收到对端 reply 时调用)
// 同时唤醒所有在等这个 IP 的 waiter
func (t *ArpTable) Insert(ip netpkg.IPv4Addr, mac MAC) {
	t.mu.Lock()
	t.entries[ip] = arpEntry{
		MAC:       mac,
		ExpiresAt: time.Now().Add(ArpTTL),
	}
	t.mu.Unlock()

	// 唤醒等这个 IP 的所有 goroutine
	t.waitersMu.Lock()
	ws := t.waiters[ip]
	delete(t.waiters, ip)
	t.waitersMu.Unlock()
	for _, w := range ws {
		select {
		case w <- mac:
		default:
		}
	}
}

// Lookup 查询缓存 (不触发广播)
func (t *ArpTable) Lookup(ip netpkg.IPv4Addr) (MAC, bool) {
	t.mu.RLock()
	defer t.mu.RUnlock()
	e, ok := t.entries[ip]
	if !ok || time.Now().After(e.ExpiresAt) {
		return MAC{}, false
	}
	return e.MAC, true
}

// Entries 快照当前所有表项 (给 `arp -a` 命令用)
func (t *ArpTable) Entries() map[netpkg.IPv4Addr]MAC {
	t.mu.RLock()
	defer t.mu.RUnlock()
	m := make(map[netpkg.IPv4Addr]MAC, len(t.entries))
	for ip, e := range t.entries {
		if time.Now().Before(e.ExpiresAt) {
			m[ip] = e.MAC
		}
	}
	return m
}

// WaitFor 为某个 IP 注册一个 waiter,返回一个 chan,
// 收到 reply 后 chan 会被写入该 MAC;超时需调用方自行处理
func (t *ArpTable) WaitFor(ip netpkg.IPv4Addr) <-chan MAC {
	ch := make(chan MAC, 1)
	t.waitersMu.Lock()
	t.waiters[ip] = append(t.waiters[ip], ch)
	t.waitersMu.Unlock()
	return ch
}

// Close 停止后台 GC
func (t *ArpTable) Close() { t.cancel() }
