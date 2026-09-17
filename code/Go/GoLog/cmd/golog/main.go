package main

import (
	"context"
	"errors"
	"flag"
	"fmt"
	"os"
	"os/signal"
	"syscall"

	"github.com/yc/golog/internal/pipeline"
	"github.com/yc/golog/internal/report"
)

func main() {
	if err := run(); err != nil {
		fmt.Fprintln(os.Stderr, "golog: "+err.Error())
		var u *report.UnknownFormat
		if errors.As(err, &u) {
			os.Exit(2)
		}
		os.Exit(3)
	}
}

func run() error {
	// 兼容 "golog stat --top 5 a.log"（子命令在前）写法：
	// 先剥离子命令名再解析 flag（默认行为即 stat 全量统计；子命令化拆分见拓展挑战 4）
	args := os.Args[1:]
	if len(args) > 0 && isSubcommand(args[0]) {
		args = args[1:]
	}

	fs := flag.NewFlagSet("golog", flag.ContinueOnError)
	top := fs.Int("top", 10, "top N items")
	format := fs.String("format", "table", "table|json|csv")
	bucket := fs.Int64("bucket", 1, "qps bucket seconds")
	if err := fs.Parse(args); err != nil {
		return err
	}

	ctx, stop := signal.NotifyContext(context.Background(), syscall.SIGINT, syscall.SIGTERM)
	defer stop()

	rest := fs.Args()
	// 兼容 "golog --top 5 stat a.log"（子命令在 flag 后）写法
	if len(rest) > 0 && isSubcommand(rest[0]) {
		rest = rest[1:]
	}

	stats := pipeline.NewStats(*bucket)
	if err := pipeline.RunFiles(ctx, rest, stats); err != nil {
		return err
	}

	rep, err := report.New(*format)
	if err != nil {
		return err
	}
	return rep.Render(os.Stdout, stats, *top)
}

func isSubcommand(s string) bool {
	switch s {
	case "stat", "top-ip", "top-url", "status", "qps":
		return true
	}
	return false
}
