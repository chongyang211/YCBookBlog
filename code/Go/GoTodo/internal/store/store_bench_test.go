package store

import (
	"path/filepath"
	"testing"
)

func BenchmarkSave_100(b *testing.B)  { benchSave(b, 100) }
func BenchmarkSave_1000(b *testing.B) { benchSave(b, 1000) }

func benchSave(b *testing.B, n int) {
	path := filepath.Join(b.TempDir(), "todo.json")
	s, _ := Open(path)
	for i := 0; i < n; i++ {
		s.Add("x", AddOption{})
	}
	b.ResetTimer()
	b.ReportAllocs()
	for i := 0; i < b.N; i++ {
		if err := s.Save(); err != nil {
			b.Fatal(err)
		}
	}
}
