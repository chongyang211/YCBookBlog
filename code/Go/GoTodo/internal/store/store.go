package store

import (
	"encoding/json"
	"errors"
	"fmt"
	"io/fs"
	"os"
	"path/filepath"
	"time"
)

// ErrNotFound is returned when a Todo with the given ID does not exist.
var ErrNotFound = errors.New("todo not found")

// fileDump is the on-disk representation of a Store.
type fileDump struct {
	Next  int    `json:"next"`
	Items []Todo `json:"items"`
}

// Open loads a Store from path. If the file does not exist, returns an empty Store.
func Open(path string) (*Store, error) {
	s := &Store{path: path, next: 1}
	data, err := os.ReadFile(path)
	if errors.Is(err, fs.ErrNotExist) {
		return s, nil
	}
	if err != nil {
		return nil, fmt.Errorf("open store %q: %w", path, err)
	}
	var dump fileDump
	if err := json.Unmarshal(data, &dump); err != nil {
		return nil, fmt.Errorf("parse store %q: %w", path, err)
	}
	s.items = dump.Items
	if dump.Next > 0 {
		s.next = dump.Next
	}
	return s, nil
}

// DefaultPath returns ~/.gotodo.json (or %USERPROFILE% on Windows).
func DefaultPath() (string, error) {
	home, err := os.UserHomeDir()
	if err != nil {
		return "", fmt.Errorf("locate home dir: %w", err)
	}
	return filepath.Join(home, ".gotodo.json"), nil
}

// Save persists the store atomically: write tmp file, fsync, then rename.
// 原子写入模板：写临时、刷盘、改名 —— 中途断电不损坏数据。
func (s *Store) Save() (err error) {
	dump := fileDump{Next: s.next, Items: s.items}

	data, err := json.MarshalIndent(dump, "", "  ")
	if err != nil {
		return fmt.Errorf("marshal store: %w", err)
	}

	dir := filepath.Dir(s.path)
	tmp, err := os.CreateTemp(dir, ".gotodo.*.tmp")
	if err != nil {
		return fmt.Errorf("create tmp file: %w", err)
	}
	tmpName := tmp.Name()

	// 即使后续 err，tmp 也要被清掉
	defer func() {
		if err != nil {
			_ = os.Remove(tmpName)
		}
	}()

	if _, err = tmp.Write(data); err != nil {
		_ = tmp.Close()
		return fmt.Errorf("write tmp: %w", err)
	}
	if err = tmp.Sync(); err != nil { // 强刷盘，防 OS 缓冲
		_ = tmp.Close()
		return fmt.Errorf("fsync tmp: %w", err)
	}
	if err = tmp.Close(); err != nil {
		return fmt.Errorf("close tmp: %w", err)
	}

	// 关键一步：rename 在同一文件系统下是原子的（POSIX）
	if err = os.Rename(tmpName, s.path); err != nil {
		return fmt.Errorf("rename %s -> %s: %w", tmpName, s.path, err)
	}
	return nil
}

// AddOption carries the optional fields of a new todo.
type AddOption struct {
	Tag string
	Due time.Time
}

// Add appends a new todo and returns it.
func (s *Store) Add(text string, opt AddOption) Todo {
	t := Todo{
		ID:        s.next,
		Text:      text,
		Tag:       opt.Tag,
		Due:       opt.Due,
		CreatedAt: time.Now(),
	}
	s.items = append(s.items, t)
	s.next++
	return t
}

// Done marks the todo with the given id as done. Idempotent.
func (s *Store) Done(id int) error {
	for i := range s.items {
		if s.items[i].ID == id {
			if s.items[i].Done {
				return nil // idempotent
			}
			s.items[i].Done = true
			s.items[i].DoneAt = time.Now()
			return nil
		}
	}
	return fmt.Errorf("done #%d: %w", id, ErrNotFound)
}

// Rm removes the todo with the given id.
func (s *Store) Rm(id int) error {
	for i := range s.items {
		if s.items[i].ID == id {
			s.items = append(s.items[:i], s.items[i+1:]...)
			return nil
		}
	}
	return fmt.Errorf("rm #%d: %w", id, ErrNotFound)
}

// Edit replaces the text of the todo with the given id.
func (s *Store) Edit(id int, text string) error {
	for i := range s.items {
		if s.items[i].ID == id {
			s.items[i].Text = text
			return nil
		}
	}
	return fmt.Errorf("edit #%d: %w", id, ErrNotFound)
}

// Clear removes all done items, returns the count removed.
func (s *Store) Clear() int {
	kept := s.items[:0] // 复用底层数组，零分配
	var n int
	for _, t := range s.items {
		if t.Done {
			n++
			continue
		}
		kept = append(kept, t)
	}
	s.items = kept
	return n
}

// Filter is the predicate type used by List.
type Filter struct {
	OnlyTodo bool
	OnlyDone bool
	Tag      string
}

// List returns the todos matching the filter, in insertion order.
func (s *Store) List(f Filter) []Todo {
	out := make([]Todo, 0, len(s.items))
	for _, t := range s.items {
		if f.OnlyTodo && t.Done {
			continue
		}
		if f.OnlyDone && !t.Done {
			continue
		}
		if f.Tag != "" && t.Tag != f.Tag {
			continue
		}
		out = append(out, t)
	}
	return out
}
