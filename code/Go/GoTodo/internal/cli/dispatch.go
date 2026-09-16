package cli

import (
	"context"
	"fmt"
	"io"
	"sort"

	"github.com/yc/gotodo/internal/store"
)

// Command is the contract every subcommand implements.
type Command interface {
	Name() string
	Synopsis() string
	Run(ctx context.Context, s *store.Store, args []string, out io.Writer) error
}

// Registry dispatches subcommands by name.
type Registry struct {
	cmds map[string]Command
}

// NewRegistry returns an empty command registry.
func NewRegistry() *Registry { return &Registry{cmds: map[string]Command{}} }

// Register adds a command to the registry.
func (r *Registry) Register(c Command) { r.cmds[c.Name()] = c }

// ErrUsage is returned for any usage / parse error. main maps it to exit code 2.
var ErrUsage = fmt.Errorf("usage error")

// Dispatch routes argv[0] to the matching command.
func (r *Registry) Dispatch(ctx context.Context, s *store.Store, argv []string, out io.Writer) error {
	if len(argv) == 0 {
		r.printUsage(out)
		return ErrUsage
	}
	name, rest := argv[0], argv[1:]
	if name == "help" || name == "-h" || name == "--help" {
		r.printUsage(out)
		return nil
	}
	c, ok := r.cmds[name]
	if !ok {
		return fmt.Errorf("unknown command %q: %w", name, ErrUsage)
	}
	return c.Run(ctx, s, rest, out)
}

func (r *Registry) printUsage(out io.Writer) {
	fmt.Fprintln(out, "Usage: gotodo <command> [flags]")
	fmt.Fprintln(out, "Commands:")
	names := make([]string, 0, len(r.cmds))
	for n := range r.cmds {
		names = append(names, n)
	}
	sort.Strings(names)
	for _, n := range names {
		fmt.Fprintf(out, "  %-10s %s\n", n, r.cmds[n].Synopsis())
	}
}
