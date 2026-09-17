package report

import (
	"bytes"
	"strings"
	"testing"
	"time"

	"github.com/yc/golog/internal/pipeline"
)

var testTime = time.Date(2026, 5, 23, 20, 30, 12, 0, time.UTC)

func newTestStats() *pipeline.Stats {
	s := pipeline.NewStats(1)
	s.TotalLines = 100
	s.BadLines = 0
	for i := 0; i < 90; i++ {
		s.IPCount.Inc("1.2.3.4")
		s.URLCount.Inc("/api")
		s.StatusBuckets[1]++ // 2xx
		s.QPS.Add(testTime)
	}
	for i := 0; i < 10; i++ {
		s.IPCount.Inc("5.6.7.8")
		s.URLCount.Inc("/health")
		s.StatusBuckets[3]++ // 4xx
	}
	return s
}

func TestTableReporter(t *testing.T) {
	rep, err := New("table")
	if err != nil {
		t.Fatal(err)
	}
	var buf bytes.Buffer
	if err := rep.Render(&buf, newTestStats(), 2); err != nil {
		t.Fatal(err)
	}
	out := buf.String()
	for _, want := range []string{"TOP 2 IPs", "1.2.3.4", "/api", "2xx", "4xx", "PEAK"} {
		if !strings.Contains(out, want) {
			t.Errorf("output missing %q:\n%s", want, out)
		}
	}
}

func TestJSONReporter(t *testing.T) {
	rep, err := New("json")
	if err != nil {
		t.Fatal(err)
	}
	var buf bytes.Buffer
	if err := rep.Render(&buf, newTestStats(), 2); err != nil {
		t.Fatal(err)
	}
	out := buf.String()
	for _, want := range []string{`"total_lines": 100`, `"top_ips"`, `"status"`, `"qps"`} {
		if !strings.Contains(out, want) {
			t.Errorf("json missing %q:\n%s", want, out)
		}
	}
}

func TestCSVReporter(t *testing.T) {
	rep, err := New("csv")
	if err != nil {
		t.Fatal(err)
	}
	var buf bytes.Buffer
	if err := rep.Render(&buf, newTestStats(), 2); err != nil {
		t.Fatal(err)
	}
	out := buf.String()
	for _, want := range []string{"section,key,value", "top_ip,1.2.3.4,90", "status,2xx,90"} {
		if !strings.Contains(out, want) {
			t.Errorf("csv missing %q:\n%s", want, out)
		}
	}
}

func TestUnknownFormat(t *testing.T) {
	if _, err := New("xml"); err == nil || !strings.Contains(err.Error(), "xml") {
		t.Fatalf("want unknown format error, got %v", err)
	}
	if _, err := New(""); err != nil {
		t.Fatalf("empty format should default to table, got %v", err)
	}
}
