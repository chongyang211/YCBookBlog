// pkg/common/hex_test.go
package common

import (
	"strings"
	"testing"
)

func TestHexdump(t *testing.T) {
	buf := []byte("Hello, MiniNet!")
	out := Hexdump(buf)
	// 应该包含 ASCII 侧栏
	if !strings.Contains(out, "|Hello, MiniNet!|") {
		t.Errorf("missing ASCII sidebar in:\n%s", out)
	}
	// 应该包含十六进制 'H' = 0x48
	if !strings.Contains(out, "48 65 6c 6c 6f") {
		t.Errorf("missing hex bytes in:\n%s", out)
	}
	t.Logf("\n%s", out) // 用 -v 看效果
}

func TestHexdumpEmpty(t *testing.T) {
	if Hexdump(nil) != "" {
		t.Error("expect empty output for nil")
	}
	if Hexdump([]byte{}) != "" {
		t.Error("expect empty output for empty slice")
	}
}

func TestHexdumpNonPrintable(t *testing.T) {
	buf := []byte{0x00, 0x01, 0xFF, 0x7F, 0x1F, 0x20, 0x41}
	out := Hexdump(buf)
	// 0x00/0x01/0xFF/0x7F/0x1F 都应该变成 '.',只有 0x20 (space) 和 0x41 ('A') 保留
	if !strings.Contains(out, "..... A|") {
		t.Errorf("non-printable chars should be dot:\n%s", out)
	}
}
