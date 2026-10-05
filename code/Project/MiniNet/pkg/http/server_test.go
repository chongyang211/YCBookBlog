package http_test

import (
	"strings"
	"testing"
	"time"

	mhttp "mininet/pkg/http"
)

// TestServerClientEndToEnd 自写 Server + 自写 Client 完整互通
func TestServerClientEndToEnd(t *testing.T) {
	mux := mhttp.NewMux()
	mux.Handle("GET", "/", func(w mhttp.ResponseWriter, r *mhttp.Request) {
		mhttp.WriteString(w, 200, "hello from mnet!")
	})
	mux.Handle("GET", "/json", func(w mhttp.ResponseWriter, r *mhttp.Request) {
		mhttp.WriteJSON(w, 200, `{"ok":true}`)
	})
	mux.Handle("POST", "/echo", func(w mhttp.ResponseWriter, r *mhttp.Request) {
		w.WriteHeader(200)
		w.Write(r.Body)
	})
	mux.Handle("GET", "/redirect", func(w mhttp.ResponseWriter, r *mhttp.Request) {
		mhttp.Redirect(w, 302, "/")
	})

	srv := mhttp.NewServer("127.0.0.1:0", mux)
	go srv.ListenAndServe()
	time.Sleep(20 * time.Millisecond)
	// 简化:上面 Addr 用 :0 会不好拿端口;重开一个显式端口
	srv.Shutdown()

	// 用 Serve + 自建 listener 拿到端口
	ln, err := netListen("tcp", "127.0.0.1:0")
	if err != nil {
		t.Fatal(err)
	}
	srv2 := mhttp.NewServer("", mux)
	go srv2.Serve(ln)
	defer srv2.Shutdown()
	time.Sleep(20 * time.Millisecond)

	addr := ln.Addr().String()
	c := &mhttp.Client{Timeout: 2 * time.Second}

	// GET /
	res, err := c.Get("http://" + addr + "/")
	if err != nil {
		t.Fatal(err)
	}
	if res.Response.StatusCode != 200 {
		t.Errorf("status = %d", res.Response.StatusCode)
	}
	if string(res.Response.Body) != "hello from mnet!" {
		t.Errorf("body = %q", res.Response.Body)
	}
	t.Logf("GET / → %s (took %v)", res.Response.Body, res.Elapsed)

	// GET /json
	res, err = c.Get("http://" + addr + "/json")
	if err != nil {
		t.Fatal(err)
	}
	if res.Response.Header.Get("Content-Type") != "application/json" {
		t.Error("CT not JSON")
	}

	// POST /echo
	res, err = c.Do("POST", "http://"+addr+"/echo", mhttp.Header{"Content-Type": "text/plain"},
		[]byte("ping"))
	if err != nil {
		t.Fatal(err)
	}
	if string(res.Response.Body) != "ping" {
		t.Errorf("echo body = %q", res.Response.Body)
	}

	// GET /redirect → 302 Location: /
	res, err = c.Get("http://" + addr + "/redirect")
	if err != nil {
		t.Fatal(err)
	}
	if res.Response.StatusCode != 302 {
		t.Errorf("redirect status = %d", res.Response.StatusCode)
	}
	if res.Response.Header.Get("Location") != "/" {
		t.Error("Location missing")
	}

	// GET /missing → 404
	res, err = c.Get("http://" + addr + "/no-such-path")
	if err != nil {
		t.Fatal(err)
	}
	if res.Response.StatusCode != 404 {
		t.Errorf("404 expected, got %d", res.Response.StatusCode)
	}
	if !strings.Contains(string(res.Response.Body), "404") {
		t.Error("404 body missing")
	}
}

func TestMuxRouting(t *testing.T) {
	m := mhttp.NewMux()
	m.Handle("GET", "/exact", func(w mhttp.ResponseWriter, r *mhttp.Request) {
		mhttp.WriteString(w, 200, "exact")
	})
	m.HandlePrefix("GET", "/api/", func(w mhttp.ResponseWriter, r *mhttp.Request) {
		mhttp.WriteString(w, 200, "api:"+r.URL)
	})

	if m.Lookup("GET", "/exact") == nil {
		t.Error("exact miss")
	}
	if m.Lookup("GET", "/api/v1/users") == nil {
		t.Error("prefix miss")
	}
	if m.Lookup("POST", "/exact") == nil {
		t.Error("fallback must not be nil")
	}
}
