package cli

import (
	"context"
	"fmt"
	"io"
	"strconv"
	"strings"

	"github.com/yc/gotodo/internal/ansi"
	"github.com/yc/gotodo/internal/store"
)

type editCmd struct{}

// NewEditCmd returns the "edit" subcommand.
func NewEditCmd() Command { return editCmd{} }

func (editCmd) Name() string     { return "edit" }
func (editCmd) Synopsis() string { return "edit the text of a todo by id" }

func (editCmd) Run(ctx context.Context, s *store.Store, args []string, out io.Writer) error {
	if len(args) < 2 {
		return fmt.Errorf("edit expects <id> <text>, got %d args: %w", len(args), ErrUsage)
	}
	id, err := strconv.Atoi(args[0])
	if err != nil {
		return fmt.Errorf("invalid id %q: %w", args[0], ErrUsage)
	}
	text := strings.TrimSpace(strings.Join(args[1:], " "))
	if text == "" {
		return fmt.Errorf("text is required: %w", ErrUsage)
	}
	if err := s.Edit(id, text); err != nil {
		return err
	}
	if err := s.Save(); err != nil {
		return fmt.Errorf("save: %w", err)
	}
	fmt.Fprintf(out, "%s #%d updated\n", ansi.Wrap(ansi.Green, "✓"), id)
	return nil
}
