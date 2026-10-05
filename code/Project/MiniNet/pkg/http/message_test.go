package http

import (
	"bufio"
	"bytes"
	"strings"
	"testing"
)

func TestReadRequestBasic(t *testing.T) {
	raw := "GET /hello?x=1 HTTP/1.1\r\n" +
		"Host: example.com\r\n" +
		"User-Agent: test\r\n" +
		"\r\n"
	r := bufio.NewReader(strings.NewReader(raw))
	req, err := ReadRequest(r)
	if err != nil {
		t.Fatal(err)
	}
	if req.Method != "GET" {
		t.Errorf("method = %q", req.Method)
	}
	if req.URL != "/hello?x=1" {
		t.Errorf("url = %q", req.URL)
	}
	if req.Header.Get("Host") != "example.com" {
		t.Errorf("host = %q", req.Header.Get("Host"))
	}
	// 大小写不敏感
	if req.Header.Get("HOST") != "example.com" {
		t.Error("case-insensitive Get failed")
	}
}

func TestReadRequestWithBody(t *testing.T) {
	raw := "POST /submit HTTP/1.1\r\n" +
		"Host: a.com\r\n" +
		"Content-Length: 5\r\n" +
		"\r\n" +
		"hello"
	r := bufio.NewReader(strings.NewReader(raw))
	req, err := ReadRequest(r)
	if err != nil {
		t.Fatal(err)
	}
	if string(req.Body) != "hello" {
		t.Errorf("body = %q", req.Body)
	}
}

func TestReadRequestChunked(t *testing.T) {
	// 3 chunks: "foo" "barbaz" "" (结束)
	raw := "POST /up HTTP/1.1\r\n" +
		"Host: a.com\r\n" +
		"Transfer-Encoding: chunked\r\n" +
		"\r\n" +
		"3\r\nfoo\r\n" +
		"6\r\nbarbaz\r\n" +
		"0\r\n" +
		"\r\n"
	r := bufio.NewReader(strings.NewReader(raw))
	req, err := ReadRequest(r)
	if err != nil {
		t.Fatal(err)
	}
	if string(req.Body) != "foobarbaz" {
		t.Errorf("chunked body = %q, want foobarbaz", req.Body)
	}
}

func TestWriteRequest(t *testing.T) {
	req := &Request{
		Method: "POST",
		URL:    "/x",
		Proto:  "HTTP/1.1",
		Header: Header{"Host": "a.com"},
		Body:   []byte("abc"),
	}
	var buf bytes.Buffer
	if _, err := req.WriteTo(&buf); err != nil {
		t.Fatal(err)
	}
	s := buf.String()
	if !strings.Contains(s, "POST /x HTTP/1.1\r\n") {
		t.Error("missing request line")
	}
	if !strings.Contains(s, "Content-Length: 3\r\n") {
		t.Error("auto Content-Length not set")
	}
	if !strings.HasSuffix(s, "abc") {
		t.Error("missing body")
	}
}

func TestReadResponse(t *testing.T) {
	raw := "HTTP/1.1 200 OK\r\n" +
		"Content-Type: text/plain\r\n" +
		"Content-Length: 2\r\n" +
		"\r\n" +
		"hi"
	r := bufio.NewReader(strings.NewReader(raw))
	resp, err := ReadResponse(r)
	if err != nil {
		t.Fatal(err)
	}
	if resp.StatusCode != 200 {
		t.Error("status code")
	}
	if resp.Header.Get("Content-Type") != "text/plain" {
		t.Error("content-type")
	}
	if string(resp.Body) != "hi" {
		t.Errorf("body = %q", resp.Body)
	}
}

func TestWriteResponse(t *testing.T) {
	resp := &Response{
		Proto:      "HTTP/1.1",
		StatusCode: 201,
		Header:     Header{"X-Test": "y"},
		Body:       []byte("ok"),
	}
	var buf bytes.Buffer
	resp.WriteTo(&buf)
	s := buf.String()
	if !strings.HasPrefix(s, "HTTP/1.1 201 Created\r\n") {
		t.Errorf("status line wrong: %q", s[:30])
	}
	if !strings.Contains(s, "Content-Length: 2\r\n") {
		t.Error("auto Content-Length")
	}
}

func TestReadRequestBadLine(t *testing.T) {
	r := bufio.NewReader(strings.NewReader("GARBAGE\r\n\r\n"))
	if _, err := ReadRequest(r); err == nil {
		t.Error("expect error")
	}
}

func TestStatusText(t *testing.T) {
	cases := map[int]string{200: "OK", 404: "Not Found", 500: "Internal Server Error", 999: "Unknown"}
	for c, want := range cases {
		if got := StatusText(c); got != want {
			t.Errorf("StatusText(%d) = %q", c, got)
		}
	}
}
