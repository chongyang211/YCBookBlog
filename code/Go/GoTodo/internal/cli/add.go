package cli

import (
	"context"
	"flag"
	"fmt"
	"io"
	"strings"
	"time"

	"github.com/yc/gotodo/internal/ansi"
	"github.com/yc/gotodo/internal/store"
)

type addCmd struct{}

// NewAddCmd returns the "add" subcommand.
func NewAddCmd() Command { return addCmd{} }

func (addCmd) Name() string     { return "add" }
func (addCmd) Synopsis() string { return "add a new todo (text required)" }

func (addCmd) Run(ctx context.Context, s *store.Store, args []string, out io.Writer) error {
	fs := flag.NewFlagSet("add", flag.ContinueOnError)
	fs.SetOutput(out)
	tag := fs.String("tag", "", "category tag, e.g. shopping")
	dueStr := fs.String("due", "", "due date, format YYYY-MM-DD")
	if err := fs.Parse(args); err != nil {
		return fmt.Errorf("parse flags: %w", ErrUsage)
	}
	text := strings.TrimSpace(strings.Join(fs.Args(), " "))
	if text == "" {
		return fmt.Errorf("text is required: %w", ErrUsage)
	}

	var due time.Time
	if *dueStr != "" {
		t, err := time.ParseInLocation("2006-01-02", *dueStr, time.Local)
		if err != nil {
			return fmt.Errorf("invalid --due %q (want YYYY-MM-DD): %w", *dueStr, ErrUsage)
		}
		due = t
	}

	t := s.Add(text, store.AddOption{Tag: *tag, Due: due})
	if err := s.Save(); err != nil {
		return fmt.Errorf("save: %w", err)
	}
	fmt.Fprintf(out, "%s added #%d %s\n",
		ansi.Wrap(ansi.Green, "✓"), t.ID, ansi.Wrap(ansi.Bold, t.Text))
	return nil
}
