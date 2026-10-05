// pkg/mux/epoll_other.go - 非 Linux 平台的 EpollReactor stub
//
// macOS / Windows / BSD 等平台编译时用这个占位,返回明确错误
// (真实 macOS 可用 kqueue,本案例教学主线在 Linux epoll,kqueue 作为延伸)
//
//go:build !linux

package mux

import (
	"errors"
	"net"
)

type EpollReactor struct{}

func NewEpollReactor(mode string) *EpollReactor { return &EpollReactor{} }

func (r *EpollReactor) Serve(ln net.Listener, h Handler) error {
	return errors.New("epoll reactor only works on Linux; try GoroutineReactor instead")
}

func (r *EpollReactor) Shutdown() error              { return nil }
func (r *EpollReactor) Stats() (int64, int64)        { return 0, 0 }
func (r *EpollReactor) ETReadLimit() int             { return 0 }
