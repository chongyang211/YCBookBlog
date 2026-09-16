package main

import (
	"bytes"
	"context"
	"path/filepath"
	"strings"
	"testing"

	"github.com/yc/gotodo/internal/cli"
	"github.com/yc/gotodo/internal/store"
)

func TestE2E_FullFlow(t *testing.T) {
	// 用独立临时文件，不污染开发者真实 ~/.gotodo.json
	path := filepath.Join(t.TempDir(), "todo.json")

	newRegistry := func() *cli.Registry {
		r := cli.NewRegistry()
		r.Register(cli.NewAddCmd())
		r.Register(cli.NewListCmd())
		r.Register(cli.NewDoneCmd())
		r.Register(cli.NewRmCmd())
		r.Register(cli.NewEditCmd())
		r.Register(cli.NewClearCmd())
		r.Register(cli.NewVersionCmd())
		return r
	}

	runCmd := func(t *testing.T, argv ...string) string {
		t.Helper()
		s, err := store.Open(path)
		if err != nil {
			t.Fatal(err)
		}
		var buf bytes.Buffer
		if err := newRegistry().Dispatch(context.Background(), s, argv, &buf); err != nil {
			t.Fatalf("cmd %v: %v", argv, err)
		}
		return buf.String()
	}

	// 端到端 8 步验收
	out := runCmd(t, "add", "task A")
	if !strings.Contains(out, "added #1") {
		t.Fatalf("add A: %s", out)
	}
	out = runCmd(t, "add", "task B", "--tag", "x", "--due", "2030-01-01")
	if !strings.Contains(out, "added #2") {
		t.Fatalf("add B: %s", out)
	}
	out = runCmd(t, "list")
	if !strings.Contains(out, "task A") || !strings.Contains(out, "task B") {
		t.Fatalf("list should show both:\n%s", out)
	}
	out = runCmd(t, "done", "1")
	if !strings.Contains(out, "#1 marked done") {
		t.Fatalf("done 1: %s", out)
	}
	out = runCmd(t, "list", "--filter", "todo")
	if !strings.Contains(out, "task B") || strings.Contains(out, "task A") {
		t.Fatalf("filter todo not respected:\n%s", out)
	}
	out = runCmd(t, "edit", "2", "task B edited")
	if !strings.Contains(out, "#2 updated") {
		t.Fatalf("edit 2: %s", out)
	}
	out = runCmd(t, "rm", "2")
	if !strings.Contains(out, "#2 removed") {
		t.Fatalf("rm 2: %s", out)
	}
	out = runCmd(t, "clear")
	if !strings.Contains(out, "cleared 1 done item") {
		t.Fatalf("clear: %s", out)
	}
	out = runCmd(t, "version")
	if !strings.Contains(out, "gotodo") {
		t.Fatalf("version: %s", out)
	}

	// 数据文件应存在且能重新打开
	s, err := store.Open(path)
	if err != nil {
		t.Fatal(err)
	}
	if got := len(s.List(store.Filter{})); got != 0 {
		t.Fatalf("after clear want 0 items, got %d", got)
	}
}
