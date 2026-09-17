package parser

import (
	"errors"
	"testing"
)

func TestParse(t *testing.T) {
	cases := []struct {
		name    string
		in      string
		wantIP  string
		wantURL string
		wantSt  int
		wantErr error
	}{
		{
			"ok",
			`192.168.1.1 - - [23/May/2026:20:30:12 +0800] "GET /api/v1/u?id=1 HTTP/1.1" 200 1234 "-" "curl"`,
			"192.168.1.1", "/api/v1/u", 200, nil,
		},
		{
			"url without query",
			`10.0.0.5 - alice [23/May/2026:20:30:12 +0800] "POST /submit HTTP/1.1" 404 0 "-" "Mozilla/5.0"`,
			"10.0.0.5", "/submit", 404, nil,
		},
		{
			"5xx",
			`172.16.0.3 - - [23/May/2026:20:30:13 +0800] "GET /health HTTP/1.1" 503 12 "-" "curl"`,
			"172.16.0.3", "/health", 503, nil,
		},
		{
			"no quotes",
			`192.168.1.1 - - [23/May/2026:20:30:12 +0800] GET / 200 0 - -`,
			"", "", 0, ErrMalformed,
		},
		{
			"bad time",
			`192.168.1.1 - - [bad-time +0800] "GET / HTTP/1.1" 200 0 "-" "-"`,
			"", "", 0, ErrMalformed,
		},
		{
			"empty",
			``,
			"", "", 0, ErrMalformed,
		},
	}
	for _, c := range cases {
		t.Run(c.name, func(t *testing.T) {
			e, err := Parse([]byte(c.in))
			if !errors.Is(err, c.wantErr) {
				t.Fatalf("err want %v got %v", c.wantErr, err)
			}
			if c.wantErr != nil {
				return
			}
			if string(e.IP) != c.wantIP {
				t.Errorf("ip want %q got %q", c.wantIP, e.IP)
			}
			if string(e.URL) != c.wantURL {
				t.Errorf("url want %q got %q", c.wantURL, e.URL)
			}
			if e.Status != c.wantSt {
				t.Errorf("status want %d got %d", c.wantSt, e.Status)
			}
		})
	}
}

func TestParse_BytesAndMethod(t *testing.T) {
	e, err := Parse([]byte(`192.168.1.1 - - [23/May/2026:20:30:12 +0800] "GET /a HTTP/1.1" 200 1234 "-" "curl"`))
	if err != nil {
		t.Fatal(err)
	}
	if string(e.Method) != "GET" {
		t.Errorf("method want GET got %q", e.Method)
	}
	if e.Bytes != 1234 {
		t.Errorf("bytes want 1234 got %d", e.Bytes)
	}
}
