package store

import (
	"errors"
	"path/filepath"
	"testing"
	"time"
)

func TestStore_AddDoneRm(t *testing.T) {
	path := filepath.Join(t.TempDir(), "todo.json")
	s, err := Open(path)
	if err != nil {
		t.Fatal(err)
	}
	a := s.Add("a", AddOption{})
	b := s.Add("b", AddOption{Tag: "x"})
	if a.ID != 1 || b.ID != 2 {
		t.Fatalf("ids want 1,2 got %d,%d", a.ID, b.ID)
	}
	if err := s.Done(1); err != nil {
		t.Fatal(err)
	}
	if err := s.Done(99); !errors.Is(err, ErrNotFound) {
		t.Fatalf("want ErrNotFound, got %v", err)
	}
	if err := s.Save(); err != nil {
		t.Fatal(err)
	}

	// reopen and verify
	s2, err := Open(path)
	if err != nil {
		t.Fatal(err)
	}
	if got := s2.List(Filter{OnlyDone: true}); len(got) != 1 || got[0].ID != 1 {
		t.Fatalf("after reopen done filter unexpected: %+v", got)
	}
}

func TestStore_FilterTable(t *testing.T) {
	s, _ := Open(filepath.Join(t.TempDir(), "todo.json"))
	s.Add("a", AddOption{Tag: "shop"})
	s.Add("b", AddOption{Tag: "work"})
	_ = s.Done(s.Add("c", AddOption{Tag: "shop"}).ID)

	cases := []struct {
		name string
		f    Filter
		want int
	}{
		{"all", Filter{}, 3},
		{"only todo", Filter{OnlyTodo: true}, 2},
		{"only done", Filter{OnlyDone: true}, 1},
		{"shop tag", Filter{Tag: "shop"}, 2},
		{"shop tag + done", Filter{Tag: "shop", OnlyDone: true}, 1},
	}
	for _, c := range cases {
		t.Run(c.name, func(t *testing.T) {
			if got := len(s.List(c.f)); got != c.want {
				t.Fatalf("want %d, got %d", c.want, got)
			}
		})
	}
}

func TestStore_AtomicSave(t *testing.T) {
	path := filepath.Join(t.TempDir(), "todo.json")
	s, _ := Open(path)
	for i := 0; i < 1000; i++ {
		s.Add("x", AddOption{Due: time.Now()})
	}
	if err := s.Save(); err != nil {
		t.Fatal(err)
	}
	// 强校验：能成功 reopen 且数量一致
	s2, _ := Open(path)
	if got := len(s2.List(Filter{})); got != 1000 {
		t.Fatalf("want 1000, got %d", got)
	}
}

func TestStore_DoneIdempotent(t *testing.T) {
	s, _ := Open(filepath.Join(t.TempDir(), "todo.json"))
	id := s.Add("a", AddOption{}).ID
	if err := s.Done(id); err != nil {
		t.Fatal(err)
	}
	if err := s.Done(id); err != nil {
		t.Fatalf("second Done should be idempotent, got %v", err)
	}
}

func TestStore_IDNotReusedAfterRm(t *testing.T) {
	s, _ := Open(filepath.Join(t.TempDir(), "todo.json"))
	a := s.Add("a", AddOption{})
	if err := s.Rm(a.ID); err != nil {
		t.Fatal(err)
	}
	b := s.Add("b", AddOption{})
	if b.ID != 2 {
		t.Fatalf("id should not be reused, want 2 got %d", b.ID)
	}
}

func TestStore_ClearReusesArray(t *testing.T) {
	s, _ := Open(filepath.Join(t.TempDir(), "todo.json"))
	s.Add("a", AddOption{})
	s.Add("b", AddOption{})
	_ = s.Done(1)
	if n := s.Clear(); n != 1 {
		t.Fatalf("want 1 cleared, got %d", n)
	}
	if got := s.List(Filter{}); len(got) != 1 || got[0].Text != "b" {
		t.Fatalf("after clear unexpected: %+v", got)
	}
}
