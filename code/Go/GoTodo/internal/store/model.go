// Package store implements the todo domain model and its JSON persistence.
package store

import "time"

// Todo represents a single task. JSON tags use lowercase to keep the file readable.
type Todo struct {
	ID        int       `json:"id"`
	Text      string    `json:"text"`
	Tag       string    `json:"tag,omitempty"`
	Due       time.Time `json:"due,omitempty"`
	Done      bool      `json:"done,omitempty"`
	CreatedAt time.Time `json:"created_at"`
	DoneAt    time.Time `json:"done_at,omitempty"`
}

// Store is the aggregate root: it owns the items and the next id to assign.
type Store struct {
	path  string
	items []Todo
	next  int // 下一个分配的 ID（单调递增，不复用）
}
