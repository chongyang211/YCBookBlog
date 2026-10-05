// pkg/link/driver.go - L2 驱动统一接口
//
// 第 2 次会话 Step 4.1
//
// 为什么要抽象 Driver 接口?
//   - loopback 模式: 两个 goroutine 用 chan 对打 (本次会话用)
//   - tap 模式    : 真实接管 utun/tap0 网卡 (后续可选)
//   - mock 测试    : 单元测试里直接灌字节
//
// 三种驱动的实现完全不同,但对上层(L2Layer/ARP/IP)而言行为一致:
//   "发出去一帧 → 收回来一帧"。接口让换驱动不改上层代码。
package link

import (
	"errors"
)

// MTU 以太网标准最大传输单元,帧 payload 最大 1500 字节
// (加上 14 字节以太头,整帧最多 1514 字节,不含 FCS)
const MTU = 1500

// ErrDriverClosed Driver 已关闭
var ErrDriverClosed = errors.New("driver closed")

// Driver 是 L2 驱动的抽象。所有实现都必须保证并发安全:
// Send 和 Recv 可能被不同的 goroutine 同时调用。
type Driver interface {
	// MAC 返回本驱动绑定的硬件地址
	MAC() MAC

	// Send 把一帧完整字节发出去 (阻塞直到对端接收或返回错误)
	// buf 包含完整的以太帧(14 字节头 + payload)
	Send(buf []byte) error

	// Recv 从驱动读取一帧完整字节 (阻塞直到有数据或返回错误)
	// 返回的 slice 由驱动所有,调用方不应持有或修改
	Recv() ([]byte, error)

	// Close 关闭驱动,之后所有 Send/Recv 返回 ErrDriverClosed
	Close() error
}
