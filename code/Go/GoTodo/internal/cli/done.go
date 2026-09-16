package cli

import (
	"context"
	"fmt"
	"io"
	"strconv"

	"github.com/yc/gotodo/internal/ansi"
	"github.com/yc/gotodo/internal/store"
)

type doneCmd struct{}

// NewDoneCmd returns the "done" subcommand.
func NewDoneCmd() Command { return doneCmd{} }

func (doneCmd) Name() string     { return "done" }
func (doneCmd) Synopsis() string { return "mark a todo done by id" }

func (doneCmd) Run(ctx context.Context, s *store.Store, args []string, out io.Writer) error {
	if len(args) != 1 {
		return fmt.Errorf("done expects 1 id, got %d: %w", len(args), ErrUsage)
	}
	id, err := strconv.Atoi(args[0])
	if err != nil {
		return fmt.Errorf("invalid id %q: %w", args[0], ErrUsage)
	}
	if err := s.Done(id); err != nil {
		return err
	}
	if err := s.Save(); err != nil {
		return fmt.Errorf("save: %w", err)
	}
	fmt.Fprintf(out, "%s #%d marked done\n", ansi.Wrap(ansi.Green, "✓"), id)
	return nil
}
