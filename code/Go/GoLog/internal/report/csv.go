package report

import (
	"encoding/csv"
	"fmt"
	"io"

	"github.com/yc/golog/internal/pipeline"
)

type csvReporter struct{}

func (csvReporter) Render(w io.Writer, s *pipeline.Stats, k int) error {
	cw := csv.NewWriter(w)

	// 每节一个 section 头 + 数据行，便于导入表格工具后按 section 分列
	if err := cw.Write([]string{"section", "key", "value"}); err != nil {
		return err
	}
	writeRow := func(section, key string, v uint64) error {
		return cw.Write([]string{section, key, fmt.Sprint(v)})
	}

	for _, it := range topKFromCounter(s.IPCount, k) {
		if err := writeRow("top_ip", it.Key, it.Count); err != nil {
			return err
		}
	}
	for _, it := range topKFromCounter(s.URLCount, k) {
		if err := writeRow("top_url", it.Key, it.Count); err != nil {
			return err
		}
	}
	labels := []string{"1xx", "2xx", "3xx", "4xx", "5xx"}
	for i, c := range s.StatusBuckets {
		if c > 0 {
			if err := writeRow("status", labels[i], c); err != nil {
				return err
			}
		}
	}
	if peakTs, peak := s.QPS.Peak(); peak > 0 {
		if err := writeRow("peak_qps", peakTs.Format("2006-01-02 15:04:05"), peak); err != nil {
			return err
		}
	}
	if err := writeRow("summary", "total_lines", s.TotalLines); err != nil {
		return err
	}
	if err := writeRow("summary", "bad_lines", s.BadLines); err != nil {
		return err
	}

	cw.Flush()
	return cw.Error()
}
