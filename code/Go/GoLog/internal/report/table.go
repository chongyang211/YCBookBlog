package report

import (
	"fmt"
	"io"
	"text/tabwriter"

	"github.com/yc/golog/internal/aggregate"
	"github.com/yc/golog/internal/pipeline"
)

type tableReporter struct{}

func (tableReporter) Render(w io.Writer, s *pipeline.Stats, k int) error {
	total := s.TotalLines - s.BadLines
	if total == 0 {
		fmt.Fprintln(w, "(no data)")
		return nil
	}

	// 1. Top IPs
	fmt.Fprintf(w, "=== TOP %d IPs ===\n", k)
	renderTopK(w, topKFromCounter(s.IPCount, k), total)

	// 2. Top URLs
	fmt.Fprintf(w, "\n=== TOP %d URLs ===\n", k)
	renderTopK(w, topKFromCounter(s.URLCount, k), total)

	// 3. Status
	fmt.Fprintln(w, "\n=== STATUS ===")
	labels := []string{"1xx", "2xx", "3xx", "4xx", "5xx"}
	tw := tabwriter.NewWriter(w, 0, 0, 2, ' ', 0)
	for i, c := range s.StatusBuckets {
		if c == 0 {
			continue
		}
		pct := float64(c) * 100 / float64(total)
		fmt.Fprintf(tw, "%s\t%d\t(%5.1f%%)\n", labels[i], c, pct)
	}
	tw.Flush()

	// 4. Peak QPS
	if peakTs, peak := s.QPS.Peak(); peak > 0 {
		fmt.Fprintf(w, "\nPEAK %s @ %d req/bucket\n", peakTs.Format("15:04:05"), peak)
	}
	return nil
}

func topKFromCounter(c *aggregate.Counter[string], k int) []aggregate.Item[string] {
	t := aggregate.NewTopK[string](k)
	c.Range(func(key string, v uint64) bool {
		t.Offer(key, v)
		return true
	})
	return t.Result()
}

func renderTopK(w io.Writer, items []aggregate.Item[string], total uint64) {
	tw := tabwriter.NewWriter(w, 0, 0, 2, ' ', 0)
	for _, it := range items {
		pct := float64(it.Count) * 100 / float64(total)
		fmt.Fprintf(tw, "%s\t%d\t(%5.1f%%)\n", it.Key, it.Count, pct)
	}
	tw.Flush()
}
