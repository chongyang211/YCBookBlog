// Package ansi provides TTY-aware ANSI color helpers.
package ansi

import (
	"os"

	"golang.org/x/term"
)

// 注：golang.org/x/term 是 Go 官方扩展，不算"第三方库"
// term.IsTerminal 跨平台（含 Windows）稳定。

const (
	Reset  = "\033[0m"
	Red    = "\033[31m"
	Green  = "\033[32m"
	Yellow = "\033[33m"
	Cyan   = "\033[36m"
	Gray   = "\033[90m"
	Bold   = "\033[1m"
)

var enabled = term.IsTerminal(int(os.Stdout.Fd()))

// Wrap colors s if stdout is a TTY, otherwise returns s unchanged.
func Wrap(color, s string) string {
	if !enabled {
		return s
	}
	return color + s + Reset
}

// Disable forcibly turns off coloring (for tests / pipes).
func Disable() { enabled = false }
