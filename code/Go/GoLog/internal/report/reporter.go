// Package report renders aggregated Stats in table/json/csv formats.
package report

import (
	"io"

	"github.com/yc/golog/internal/pipeline"
)

// Reporter renders stats to w. topK controls how many top items to show.
type Reporter interface {
	Render(w io.Writer, s *pipeline.Stats, topK int) error
}

// New returns the Reporter for the given format ("table" if empty).
func New(format string) (Reporter, error) {
	switch format {
	case "table", "":
		return tableReporter{}, nil
	case "json":
		return jsonReporter{}, nil
	case "csv":
		return csvReporter{}, nil
	default:
		return nil, &UnknownFormat{Format: format}
	}
}

// UnknownFormat is returned by New for an unsupported --format value.
type UnknownFormat struct{ Format string }

func (e *UnknownFormat) Error() string { return "unknown format: " + e.Format }
