package cli

import (
	"context"
	"fmt"
	"io"
	"strconv"

	"github.com/yc/gotodo/internal/ansi"
	"github.com/yc/gotodo/internal/store"
)

type rmCmd struct{}

// NewRmCmd returns the "rm" subcommand.
func NewRmCmd() Command { return rmCmd{} }

func (rmCmd) Name() string     { return "rm" }
func (rmCmd) Synopsis() string { return "remove a todo by id" }

func (rmCmd) Run(ctx context.Context, s *store.Store, args []string, out io.Writer) error {
	if len(args) != 1 {
		return fmt.Errorf("rm expects 1 id, got %d: %w", len(args), ErrUsage)
	}
	id, err := strconv.Atoi(args[0])
	if err != nil {
		return fmt.Errorf("invalid id %q: %w", args[0], ErrUsage)
	}
	if err := s.Rm(id); err != nil {
		return err
	}
	if err := s.Save(); err != nil {
		return fmt.Errorf("save: %w", err)
	}
	fmt.Fprintf(out, "%s #%d removed\n", ansi.Wrap(ansi.Green, "✓"), id)
	return nil
}
