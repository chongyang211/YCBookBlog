package cli

import (
	"context"
	"flag"
	"fmt"
	"io"
	"text/tabwriter"
	"time"

	"github.com/yc/gotodo/internal/ansi"
	"github.com/yc/gotodo/internal/store"
)

type listCmd struct{}

// NewListCmd returns the "list" subcommand.
func NewListCmd() Command { return listCmd{} }

func (listCmd) Name() string     { return "list" }
func (listCmd) Synopsis() string { return "list todos with optional filters" }

func (listCmd) Run(ctx context.Context, s *store.Store, args []string, out io.Writer) error {
	fs := flag.NewFlagSet("list", flag.ContinueOnError)
	fs.SetOutput(out)
	filter := fs.String("filter", "", "todo|done; default all")
	tag := fs.String("tag", "", "filter by tag")
	if err := fs.Parse(args); err != nil {
		return fmt.Errorf("parse flags: %w", ErrUsage)
	}
	f := store.Filter{Tag: *tag}
	switch *filter {
	case "todo":
		f.OnlyTodo = true
	case "done":
		f.OnlyDone = true
	case "":
		// all
	default:
		return fmt.Errorf("invalid --filter %q (want todo|done): %w", *filter, ErrUsage)
	}

	items := s.List(f)
	if len(items) == 0 {
		fmt.Fprintln(out, ansi.Wrap(ansi.Gray, "(no todos)"))
		return nil
	}

	tw := tabwriter.NewWriter(out, 0, 0, 2, ' ', 0)
	fmt.Fprintln(tw, "ID\tSTATE\tTAG\tDUE\tTEXT")
	for _, t := range items {
		state := ansi.Wrap(ansi.Yellow, "○ todo")
		if t.Done {
			state = ansi.Wrap(ansi.Green, "● done")
		}
		due := "-"
		if !t.Due.IsZero() {
			due = t.Due.Format("2006-01-02")
			if !t.Done && t.Due.Before(time.Now()) {
				due = ansi.Wrap(ansi.Red, due+" !")
			}
		}
		tag := t.Tag
		if tag == "" {
			tag = "-"
		}
		fmt.Fprintf(tw, "#%d\t%s\t%s\t%s\t%s\n", t.ID, state, tag, due, t.Text)
	}
	return tw.Flush()
}
