package cli

import (
	"context"
	"fmt"
	"io"
	"runtime"

	"github.com/yc/gotodo/internal/store"
)

// Version is the current gotodo version.
const Version = "0.1.0"

type versionCmd struct{}

// NewVersionCmd returns the "version" subcommand.
func NewVersionCmd() Command { return versionCmd{} }

func (versionCmd) Name() string     { return "version" }
func (versionCmd) Synopsis() string { return "print version info" }

func (versionCmd) Run(ctx context.Context, s *store.Store, args []string, out io.Writer) error {
	fmt.Fprintf(out, "gotodo %s (%s/%s, go %s)\n",
		Version, runtime.GOOS, runtime.GOARCH, runtime.Version())
	return nil
}
