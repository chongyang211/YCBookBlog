// pkg/quic/conn.go - MiniQUIC Conn + 多流复用 + ConnID 迁移
//
// 第 7 次会话 Step 15.2 + 15.3
//
// 设计:
//   • Conn 底层是 net.PacketConn (UDP), 用 ConnID 识别对端 (不依赖五元组)
//   • 每个 Conn 内可开多个 Stream, 并发收发不阻塞 (对比 TCP 队头阻塞)
//   • 服务端维护 map[ConnID]*Conn, 对端换了地址 (迁移), 服务端通过 ConnID 继续
//
// 本教学版省略:
//   • 密码学 (真 QUIC 内嵌 TLS 1.3)
//   • 丢包恢复细节 (ACK 简化成 "最大已收")
//   • 流控 (假设内存无限)
//   • 多路径同时激活
package quic

import (
	"errors"
	"fmt"
	"io"
	"net"
	"sync"
	"sync/atomic"
	"time"

	"mininet/pkg/common"
)

// Conn 一个 QUIC "逻辑连接"
// 可绑定多个 Stream, ConnID 不变的情况下底层 UDP 对端地址可切换
type Conn struct {
	pc        net.PacketConn
	connID    ConnID
	peerAddr  atomic.Pointer[net.UDPAddr] // 当前对端地址 (迁移时会变)
	isServer  bool

	nextPktNum atomic.Uint32
	nextSID    atomic.Uint32 // 客户端=偶数, 服务端=奇数
	streams    sync.Map       // stream_id → *Stream

	mu     sync.Mutex
	closed bool

	// 对端新开的流的通知 (服务端用)
	incomingStreamCh chan *Stream

	// Path validation
	pendingChallenge map[[8]byte]chan struct{}
	chalMu           sync.Mutex
}

// Stream 单向字节流 (教学简化: 不分 uni/bi, 统一双向)
type Stream struct {
	ID     uint32
	conn   *Conn
	recvMu sync.Mutex
	recv   []byte
	closed atomic.Bool
	finRcv atomic.Bool
	// 简易可读通知
	readSignal chan struct{}

	sendOffset uint64
}

// -------- Dial / Listen --------

// Dial 客户端:连到 udpAddr
func Dial(udpAddr string) (*Conn, error) {
	raddr, err := net.ResolveUDPAddr("udp", udpAddr)
	if err != nil {
		return nil, err
	}
	pc, err := net.ListenUDP("udp", nil)
	if err != nil {
		return nil, err
	}
	var cid ConnID
	// 教学简化: ConnID 用时间戳,真 QUIC 用随机 + 握手协商
	nsec := uint64(time.Now().UnixNano())
	for i := 0; i < 8; i++ {
		cid[i] = byte(nsec >> (8 * i))
	}
	c := newConn(pc, cid, false)
	c.peerAddr.Store(raddr)
	go c.readLoop()
	// 发个 PING 让服务端建会话
	c.sendFrames([]Frame{&PingFrame{}})
	return c, nil
}

// Listener 服务端
type Listener struct {
	pc      net.PacketConn
	acceptC chan *Conn
	conns   sync.Map // ConnID → *Conn
	closed  atomic.Bool
}

// Listen 服务端:监听 UDP addr
func Listen(udpAddr string) (*Listener, error) {
	pc, err := net.ListenPacket("udp", udpAddr)
	if err != nil {
		return nil, err
	}
	ln := &Listener{pc: pc, acceptC: make(chan *Conn, 16)}
	go ln.readLoop()
	return ln, nil
}

// Accept 阻塞等待新连接
func (l *Listener) Accept() (*Conn, error) {
	c, ok := <-l.acceptC
	if !ok {
		return nil, io.EOF
	}
	return c, nil
}

// Addr 本地监听地址
func (l *Listener) Addr() net.Addr { return l.pc.LocalAddr() }

// Close
func (l *Listener) Close() error {
	l.closed.Store(true)
	close(l.acceptC)
	return l.pc.Close()
}

// 服务端循环: 收包 → 按 ConnID 分流
func (l *Listener) readLoop() {
	buf := make([]byte, 2048)
	for {
		n, raddr, err := l.pc.ReadFrom(buf)
		if err != nil {
			if l.closed.Load() {
				return
			}
			common.Trace("quic", "listener read: %v", err)
			return
		}
		pkt, err := Decode(buf[:n])
		if err != nil {
			common.Trace("quic", "decode: %v", err)
			continue
		}
		udpAddr, _ := raddr.(*net.UDPAddr)
		var conn *Conn
		if v, ok := l.conns.Load(pkt.ConnID); ok {
			conn = v.(*Conn)
			// 🔑 迁移: 对端地址变了 → 发 PATH_CHALLENGE 验证
			if cur := conn.peerAddr.Load(); cur != nil &&
				(cur.IP.String() != udpAddr.IP.String() || cur.Port != udpAddr.Port) {
				common.Info("quic", "📡 ConnID %s 迁移 %s → %s",
					pkt.ConnID, cur, udpAddr)
				conn.initiateMigration(udpAddr)
			}
		} else {
			conn = newConn(l.pc, pkt.ConnID, true)
			conn.peerAddr.Store(udpAddr)
			l.conns.Store(pkt.ConnID, conn)
			common.Info("quic", "quic server: new conn %s from %s", pkt.ConnID, udpAddr)
			select {
			case l.acceptC <- conn:
			default:
			}
		}
		conn.handlePacket(pkt)
	}
}

// -------- Conn 核心 --------

func newConn(pc net.PacketConn, cid ConnID, isServer bool) *Conn {
	c := &Conn{
		pc:               pc,
		connID:           cid,
		isServer:         isServer,
		incomingStreamCh: make(chan *Stream, 64),
		pendingChallenge: make(map[[8]byte]chan struct{}),
	}
	if isServer {
		c.nextSID.Store(1) // server 奇数
	} else {
		c.nextSID.Store(0) // client 偶数
	}
	return c
}

// ConnID 本连接 ID
func (c *Conn) ConnID() ConnID { return c.connID }

// LocalAddr 底层 UDP socket 本地地址
func (c *Conn) LocalAddr() net.Addr { return c.pc.LocalAddr() }

// PeerAddr 当前对端地址
func (c *Conn) PeerAddr() net.Addr {
	if a := c.peerAddr.Load(); a != nil {
		return a
	}
	return nil
}

// OpenStream 开新流
func (c *Conn) OpenStream() *Stream {
	// 偶/奇数交替
	id := c.nextSID.Add(2) - 2
	s := &Stream{
		ID:         id,
		conn:       c,
		readSignal: make(chan struct{}, 1),
	}
	c.streams.Store(id, s)
	return s
}

// AcceptStream 阻塞等待对端新开的流
func (c *Conn) AcceptStream(timeout time.Duration) *Stream {
	select {
	case s := <-c.incomingStreamCh:
		return s
	case <-time.After(timeout):
		return nil
	}
}

func (c *Conn) nextPkt() uint32 {
	return c.nextPktNum.Add(1) - 1
}

func (c *Conn) sendFrames(frames []Frame) error {
	pkt := &Packet{
		ConnID:    c.connID,
		PacketNum: c.nextPkt(),
		Frames:    frames,
	}
	buf := pkt.Encode()
	addr := c.peerAddr.Load()
	if addr == nil {
		return errors.New("no peer addr")
	}
	_, err := c.pc.WriteTo(buf, addr)
	return err
}

// Client-side readLoop
func (c *Conn) readLoop() {
	buf := make([]byte, 2048)
	for {
		n, _, err := c.pc.ReadFrom(buf)
		if err != nil {
			if c.closed {
				return
			}
			common.Trace("quic", "client read: %v", err)
			return
		}
		pkt, err := Decode(buf[:n])
		if err != nil {
			continue
		}
		c.handlePacket(pkt)
	}
}

func (c *Conn) handlePacket(pkt *Packet) {
	for _, f := range pkt.Frames {
		switch x := f.(type) {
		case *StreamFrame:
			sv, ok := c.streams.Load(x.StreamID)
			var s *Stream
			if !ok {
				// 对端新开的流 → 建后推入 incomingStreamCh
				s = &Stream{
					ID: x.StreamID, conn: c,
					readSignal: make(chan struct{}, 1),
				}
				actual, loaded := c.streams.LoadOrStore(x.StreamID, s)
				if loaded {
					s = actual.(*Stream)
				} else {
					// 真正新开 → 通知 Accept
					select {
					case c.incomingStreamCh <- s:
					default: // 满了丢
					}
				}
			} else {
				s = sv.(*Stream)
			}
			s.recvMu.Lock()
			s.recv = append(s.recv, x.Data...)
			if x.Fin {
				s.finRcv.Store(true)
			}
			s.recvMu.Unlock()
			select {
			case s.readSignal <- struct{}{}:
			default:
			}
		case *PingFrame:
			// 回 ACK
			c.sendFrames([]Frame{&ACKFrame{Largest: pkt.PacketNum}})
		case *PathChallengeFrame:
			// 回 PATH_RESPONSE (证明路径可用)
			c.sendFrames([]Frame{&PathResponseFrame{Token: x.Token}})
		case *PathResponseFrame:
			c.chalMu.Lock()
			if ch, ok := c.pendingChallenge[x.Token]; ok {
				close(ch)
				delete(c.pendingChallenge, x.Token)
			}
			c.chalMu.Unlock()
		case *ACKFrame:
			// 教学简化:不处理 (真实 QUIC 用于拥塞/重传)
		}
	}
}

// initiateMigration 服务端发起 Path Challenge 验证新路径
// 在新路径验证通过前,服务端可以暂时使用新路径回写(本案例简化: 直接切换)
func (c *Conn) initiateMigration(newAddr *net.UDPAddr) {
	// 先切换到新地址
	c.peerAddr.Store(newAddr)
	// 发 PATH_CHALLENGE
	var token [8]byte
	nsec := uint64(time.Now().UnixNano())
	for i := 0; i < 8; i++ {
		token[i] = byte(nsec >> (8 * i))
	}
	done := make(chan struct{})
	c.chalMu.Lock()
	c.pendingChallenge[token] = done
	c.chalMu.Unlock()
	c.sendFrames([]Frame{&PathChallengeFrame{Token: token}})
	go func() {
		select {
		case <-done:
			common.Info("quic", "✅ Path validated %s", newAddr)
		case <-time.After(3 * time.Second):
			common.Info("quic", "⚠ Path challenge timeout for %s", newAddr)
		}
	}()
}

// MigrateTo 客户端主动迁移: 换一个本地 UDP socket (模拟 Wi-Fi 切 4G)
func (c *Conn) MigrateTo() error {
	newPC, err := net.ListenUDP("udp", nil)
	if err != nil {
		return err
	}
	c.mu.Lock()
	oldPC := c.pc
	c.pc = newPC
	c.mu.Unlock()
	oldPC.Close()
	// 新 socket 发个 Ping,让服务端看到新源地址
	go c.readLoop()
	return c.sendFrames([]Frame{&PingFrame{}})
}

// Close
func (c *Conn) Close() error {
	c.mu.Lock()
	if c.closed {
		c.mu.Unlock()
		return nil
	}
	c.closed = true
	pc := c.pc
	c.mu.Unlock()
	// 服务端的 pc 是共享的,别关
	if !c.isServer && pc != nil {
		pc.Close()
	}
	return nil
}

// -------- Stream 接口 --------

// Write 发数据 (非阻塞,立即发一个 UDP 包)
func (s *Stream) Write(p []byte) (int, error) {
	if s.closed.Load() {
		return 0, errors.New("stream closed")
	}
	sf := &StreamFrame{
		StreamID: s.ID,
		Offset:   s.sendOffset,
		Data:     p,
	}
	s.sendOffset += uint64(len(p))
	err := s.conn.sendFrames([]Frame{sf})
	return len(p), err
}

// WriteFin 发完数据并关闭写端
func (s *Stream) WriteFin(p []byte) error {
	sf := &StreamFrame{
		StreamID: s.ID,
		Offset:   s.sendOffset,
		Data:     p,
		Fin:      true,
	}
	s.sendOffset += uint64(len(p))
	return s.conn.sendFrames([]Frame{sf})
}

// Read 读数据 (阻塞直到有数据或超时)
func (s *Stream) Read(p []byte) (int, error) {
	for {
		s.recvMu.Lock()
		if len(s.recv) > 0 {
			n := copy(p, s.recv)
			s.recv = s.recv[n:]
			s.recvMu.Unlock()
			return n, nil
		}
		if s.finRcv.Load() {
			s.recvMu.Unlock()
			return 0, io.EOF
		}
		s.recvMu.Unlock()
		select {
		case <-s.readSignal:
		case <-time.After(5 * time.Second):
			return 0, fmt.Errorf("stream read timeout")
		}
	}
}

// Close 流关闭
func (s *Stream) Close() error {
	s.closed.Store(true)
	return nil
}
