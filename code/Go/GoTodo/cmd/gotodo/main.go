package main

import (
	"context"
	"errors"
	"fmt"
	"os"
	"os/signal"
	"syscall"

	"github.com/yc/gotodo/internal/cli"
	"github.com/yc/gotodo/internal/store"
)

func main() {
	if err := run(); err != nil {
		fmt.Fprintln(os.Stderr, "gotodo: "+err.Error())
		os.Exit(exitCode(err))
	}
}

func run() error {
	path, err := store.DefaultPath()
	if err != nil {
		return err
	}
	s, err := store.Open(path)
	if err != nil {
		return err
	}

	ctx, stop := signal.NotifyContext(context.Background(), syscall.SIGINT, syscall.SIGTERM)
	defer stop()

	r := cli.NewRegistry()
	r.Register(cli.NewAddCmd())
	r.Register(cli.NewListCmd())
	r.Register(cli.NewDoneCmd())
	r.Register(cli.NewRmCmd())
	r.Register(cli.NewEditCmd())
	r.Register(cli.NewClearCmd())
	r.Register(cli.NewVersionCmd())

	return r.Dispatch(ctx, s, os.Args[1:], os.Stdout)
}

// exitCode translates semantic errors into Unix exit codes:
// 0=ok, 2=usage, 3=io, 4=not found.
func exitCode(err error) int {
	switch {
	case errors.Is(err, cli.ErrUsage):
		return 2
	case errors.Is(err, store.ErrNotFound):
		return 4
	default:
		return 3
	}
}
