// pkg/common/hex.go - 仿 hexdump -C 格式的字节打印
package common

import (
	"fmt"
	"strings"
)

// Hexdump 把 buf 格式化成 hexdump -C 风格的字符串
// 每行 16 字节: 偏移 + 十六进制 + ASCII 侧栏
//
// 示例:
//
//	00000000  48 65 6c 6c 6f 2c 20 4d  69 6e 69 4e 65 74 21     |Hello, MiniNet!|
func Hexdump(buf []byte) string {
	var b strings.Builder
	for off := 0; off < len(buf); off += 16 {
		end := off + 16
		if end > len(buf) {
			end = len(buf)
		}
		row := buf[off:end]

		// 偏移地址
		fmt.Fprintf(&b, "%08x  ", off)

		// 十六进制: 每 8 字节中间加个额外空格
		for i := 0; i < 16; i++ {
			if i < len(row) {
				fmt.Fprintf(&b, "%02x ", row[i])
			} else {
				b.WriteString("   ")
			}
			if i == 7 {
				b.WriteByte(' ')
			}
		}

		// ASCII 侧栏:只打印可见字符,非打印字符一律 '.'
		b.WriteString(" |")
		for _, c := range row {
			if c >= 0x20 && c <= 0x7E {
				b.WriteByte(c)
			} else {
				b.WriteByte('.')
			}
		}
		b.WriteString("|\n")
	}
	return b.String()
}
