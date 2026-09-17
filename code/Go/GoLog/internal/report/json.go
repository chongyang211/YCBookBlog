package report

import (
	"encoding/json"
	"io"
	"time"

	"github.com/yc/golog/internal/pipeline"
)

type jsonReporter struct{}

type jsonItem struct {
	Key   string `json:"key"`
	Count uint64 `json:"count"`
}

type jsonStatus struct {
	Class string `json:"class"`
	Count uint64 `json:"count"`
}

type jsonQPSBucket struct {
	Time  string `json:"time"`
	Count uint64 `json:"count"`
}

type jsonReport struct {
	TotalLines uint64       `json:"total_lines"`
	BadLines   uint64       `json:"bad_lines"`
	TopIPs     []jsonItem   `json:"top_ips"`
	TopURLs    []jsonItem   `json:"top_urls"`
	Status     []jsonStatus `json:"status"`
	QPS        struct {
		PeakTime  string          `json:"peak_time,omitempty"`
		PeakCount uint64          `json:"peak_count,omitempty"`
		Buckets   []jsonQPSBucket `json:"buckets"`
	} `json:"qps"`
}

func (jsonReporter) Render(w io.Writer, s *pipeline.Stats, k int) error {
	r := jsonReport{
		TotalLines: s.TotalLines,
		BadLines:   s.BadLines,
	}
	for _, it := range topKFromCounter(s.IPCount, k) {
		r.TopIPs = append(r.TopIPs, jsonItem{Key: it.Key, Count: it.Count})
	}
	for _, it := range topKFromCounter(s.URLCount, k) {
		r.TopURLs = append(r.TopURLs, jsonItem{Key: it.Key, Count: it.Count})
	}
	labels := []string{"1xx", "2xx", "3xx", "4xx", "5xx"}
	for i, c := range s.StatusBuckets {
		if c > 0 {
			r.Status = append(r.Status, jsonStatus{Class: labels[i], Count: c})
		}
	}
	if peakTs, peak := s.QPS.Peak(); peak > 0 {
		r.QPS.PeakTime = peakTs.Format(time.RFC3339)
		r.QPS.PeakCount = peak
	}
	ts, counts := s.QPS.Sorted()
	for i := range ts {
		r.QPS.Buckets = append(r.QPS.Buckets,
			jsonQPSBucket{Time: ts[i].Format(time.RFC3339), Count: counts[i]})
	}

	enc := json.NewEncoder(w)
	enc.SetIndent("", "  ")
	return enc.Encode(r)
}
