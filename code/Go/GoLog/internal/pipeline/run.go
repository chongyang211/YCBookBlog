// Package pipeline wires Reader → Parser → Aggregator into a streaming pass.
package pipeline

import (
	"bufio"
	"context"
	"fmt"
	"io"
	"os"

	"github.com/yc/golog/internal/aggregate"
	"github.com/yc/golog/internal/parser"
)

// Stats is the aggregated result.
type Stats struct {
	IPCount      *aggregate.Counter[string]
	URLCount     *aggregate.Counter[string]
	StatusBuckets [5]uint64 // 1xx..5xx
	QPS          *aggregate.QPSBuckets
	TotalLines   uint64
	BadLines     uint64
}

// NewStats returns an empty Stats with the given QPS bucket size (seconds).
func NewStats(qpsBucketSec int64) *Stats {
	return &Stats{
		IPCount:  aggregate.NewCounter[string](),
		URLCount: aggregate.NewCounter[string](),
		QPS:      aggregate.NewQPSBuckets(qpsBucketSec),
	}
}

// Run feeds lines from r into stats. ctx cancellation aborts mid-stream.
func Run(ctx context.Context, r io.Reader, stats *Stats) error {
	sc := bufio.NewScanner(r)
	sc.Buffer(make([]byte, 64*1024), 1<<20) // 64 KB init, 1 MB max line

	for sc.Scan() {
		select {
		case <-ctx.Done():
			return ctx.Err()
		default:
		}
		stats.TotalLines++
		e, err := parser.Parse(sc.Bytes())
		if err != nil {
			stats.BadLines++
			continue
		}
		// KEY: 在这里 string() 转换才是真正"持久化"——key 进入 map 就必须复制
		stats.IPCount.Inc(string(e.IP))
		stats.URLCount.Inc(string(e.URL))

		// status bucket
		if e.Status >= 100 && e.Status < 600 {
			stats.StatusBuckets[e.Status/100-1]++
		}
		stats.QPS.Add(e.Time)
	}
	if err := sc.Err(); err != nil {
		return fmt.Errorf("scan: %w", err)
	}
	return nil
}

// RunFiles processes one or more files (or stdin if files is empty).
func RunFiles(ctx context.Context, files []string, stats *Stats) error {
	if len(files) == 0 {
		return Run(ctx, os.Stdin, stats)
	}
	for _, f := range files {
		if err := runOne(ctx, f, stats); err != nil {
			return err
		}
	}
	return nil
}

func runOne(ctx context.Context, path string, stats *Stats) (err error) {
	f, err := os.Open(path)
	if err != nil {
		return fmt.Errorf("open %s: %w", path, err)
	}
	defer func() {
		if cerr := f.Close(); err == nil {
			err = cerr
		}
	}()
	return Run(ctx, f, stats)
}
