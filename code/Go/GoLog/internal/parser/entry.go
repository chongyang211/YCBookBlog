// Package parser implements zero-allocation parsing of Nginx combined logs.
package parser

import "time"

// Entry holds one parsed log line. All fields are SLICES INTO the original buffer
// — caller must NOT retain them across iterations.
type Entry struct {
	IP     []byte
	Time   time.Time
	Method []byte
	URL    []byte
	Status int
	Bytes  int64
}
