package parser

import (
	"strconv"
	"strings"
	"testing"
	"time"
)

var benchLine = []byte(`192.168.1.1 - - [23/May/2026:20:30:12 +0800] "GET /api/v1/u?id=1 HTTP/1.1" 200 1234 "-" "curl"`)

func BenchmarkParse(b *testing.B) {
	b.ReportAllocs()
	for i := 0; i < b.N; i++ {
		if _, err := Parse(benchLine); err != nil {
			b.Fatal(err)
		}
	}
}

// Naive baseline using strings.Split for comparison.
func BenchmarkParseSlow(b *testing.B) {
	b.ReportAllocs()
	line := string(benchLine)
	for i := 0; i < b.N; i++ {
		_ = parseSlow(line)
	}
}

func parseSlow(line string) (e Entry) {
	parts := strings.SplitN(line, " ", 12)
	e.IP = []byte(parts[0])
	ts, _ := time.Parse("[02/Jan/2006:15:04:05 -0700]", parts[3]+" "+parts[4])
	e.Time = ts
	rq := strings.Trim(parts[5]+" "+parts[6]+" "+parts[7], `"`)
	rp := strings.SplitN(rq, " ", 3)
	e.Method, e.URL = []byte(rp[0]), []byte(rp[1])
	e.Status, _ = strconv.Atoi(parts[8])
	return
}
