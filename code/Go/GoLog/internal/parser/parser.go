package parser

import (
	"bytes"
	"errors"
	"strconv"
	"time"
)

var (
	// ErrMalformed is returned for any line that does not match the
	// Nginx combined log format.
	ErrMalformed = errors.New("malformed log line")
	timeLayout   = "02/Jan/2006:15:04:05 -0700"
)

// Parse parses one Nginx combined log line. The returned Entry's []byte fields
// share memory with `line`; do NOT retain them across calls.
//
// 格式：$remote_addr - $remote_user [$time_local] "$request" $status
//       $body_bytes_sent "$http_referer" "$http_user_agent"
func Parse(line []byte) (Entry, error) {
	var e Entry

	// 1) IP — first space-separated field
	sp := bytes.IndexByte(line, ' ')
	if sp < 0 {
		return e, ErrMalformed
	}
	e.IP = line[:sp]
	rest := line[sp+1:]

	// 2) Skip "- -" (remote_user / ident)
	rest = skipFields(rest, 2)
	if rest == nil {
		return e, ErrMalformed
	}

	// 3) Time — between '[' and ']'
	if len(rest) == 0 || rest[0] != '[' {
		return e, ErrMalformed
	}
	end := bytes.IndexByte(rest, ']')
	if end < 0 {
		return e, ErrMalformed
	}
	ts, err := time.Parse(timeLayout, string(rest[1:end]))
	if err != nil {
		return e, ErrMalformed
	}
	e.Time = ts
	rest = rest[end+2:] // skip "] "

	// 4) Request — between '"' and '"'  e.g. "GET /api?x HTTP/1.1"
	if len(rest) == 0 || rest[0] != '"' {
		return e, ErrMalformed
	}
	rest = rest[1:]
	rqEnd := bytes.IndexByte(rest, '"')
	if rqEnd < 0 {
		return e, ErrMalformed
	}
	request := rest[:rqEnd]
	rest = rest[rqEnd+2:] // skip `" `

	// 4.1) Method
	s := bytes.IndexByte(request, ' ')
	if s < 0 {
		return e, ErrMalformed
	}
	e.Method = request[:s]
	request = request[s+1:]

	// 4.2) URL — strip query string for aggregation
	s = bytes.IndexByte(request, ' ')
	if s < 0 {
		return e, ErrMalformed
	}
	url := request[:s]
	if q := bytes.IndexByte(url, '?'); q >= 0 {
		url = url[:q]
	}
	e.URL = url

	// 5) Status
	sp2 := bytes.IndexByte(rest, ' ')
	if sp2 < 0 {
		return e, ErrMalformed
	}
	status, err := atoiBytes(rest[:sp2])
	if err != nil {
		return e, ErrMalformed
	}
	e.Status = status
	rest = rest[sp2+1:]

	// 6) Body bytes
	sp3 := bytes.IndexByte(rest, ' ')
	if sp3 < 0 {
		sp3 = len(rest)
	}
	n, err := strconv.ParseInt(string(rest[:sp3]), 10, 64)
	if err != nil {
		return e, ErrMalformed
	}
	e.Bytes = n

	return e, nil
}

// skipFields skips n space-separated fields, returns the remainder (or nil).
func skipFields(b []byte, n int) []byte {
	for i := 0; i < n; i++ {
		sp := bytes.IndexByte(b, ' ')
		if sp < 0 {
			return nil
		}
		b = b[sp+1:]
	}
	return b
}

// atoiBytes is a minimal []byte -> int that avoids string conversion.
func atoiBytes(b []byte) (int, error) {
	if len(b) == 0 {
		return 0, ErrMalformed
	}
	n := 0
	for _, c := range b {
		if c < '0' || c > '9' {
			return 0, ErrMalformed
		}
		n = n*10 + int(c-'0')
	}
	return n, nil
}
