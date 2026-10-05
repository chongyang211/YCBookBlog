// pkg/mux/fdhelper_linux.go - os.NewFile 的本地别名
//go:build linux

package mux

import "os"

func osNewFile(fd uintptr, name string) *os.File { return os.NewFile(fd, name) }
