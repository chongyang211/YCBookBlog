package cli

import (
	"context"
	"fmt"
	"io"

	"github.com/yc/gotodo/internal/ansi"
	"github.com/yc/gotodo/internal/store"
)

type clearCmd struct{}

// NewClearCmd returns the "clear" subcommand.
func NewClearCmd() Command { return clearCmd{} }

func (clearCmd) Name() string     { return "clear" }
func (clearCmd) Synopsis() string { return "remove all done todos" }

func (clearCmd) Run(ctx context.Context, s *store.Store, args []string, out io.Writer) error {
	n := s.Clear()
	if err := s.Save(); err != nil {
		return fmt.Errorf("save: %w", err)
	}
	fmt.Fprintf(out, "%s cleared %d done item(s)\n", ansi.Wrap(ansi.Green, "✓"), n)
	return nil
}
