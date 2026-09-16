# gotodo

命令行待办事项管理器 —— Go 综合案例 01，零第三方依赖（仅官方扩展 `golang.org/x/term`）。

## 快速开始

```bash
# 本地运行
go run ./cmd/gotodo add "学 Go"

# 跑测试
go test ./... -race -v

# 全局安装（前提：$GOPATH/bin 在 PATH）
go install ./cmd/gotodo
```

## 命令一览

| 命令 | 作用 | 示例 |
|---|---|---|
| `add` | 新增任务 | `gotodo add "买牛奶" --tag shopping --due 2026-06-01` |
| `list` | 列出任务 | `gotodo list --filter todo --tag shopping` |
| `done` | 标记完成 | `gotodo done 3` |
| `rm` | 删除任务 | `gotodo rm 3` |
| `edit` | 修改任务 | `gotodo edit 3 "买全脂牛奶"` |
| `clear` | 清空已完成 | `gotodo clear` |
| `version` | 输出版本信息 | `gotodo version` |

> 注意：Go 标准库 `flag` 要求标志参数放在位置参数**之前**，即 `gotodo add --tag shopping --due 2026-06-01 买牛奶`（而不是 `gotodo add 买牛奶 --tag shopping`）。

## 设计要点

- 数据持久化到 `~/.gotodo.json`，**原子写入**（写临时文件 + `Sync` + `os.Rename`）
- 彩色输出（ANSI 转义码，TTY 自动检测，管道/重定向自动关闭颜色）
- 退出码语义化：`0` 成功、`2` 用户输入错误、`3` 文件 IO 错误、`4` 任务不存在
- 全部错误 `%w` 链式包装，可被 `errors.Is/As` 解构

## 目录结构

```
gotodo/
├── go.mod
├── cmd/gotodo/main.go          # 入口：run() + 退出码映射
├── internal/
│   ├── cli/                    # 子命令分发（Command 接口 + Registry）
│   │   ├── dispatch.go
│   │   ├── add.go / list.go / done.go / rm.go / edit.go / clear.go / version.go
│   ├── store/                  # 业务逻辑 + 原子持久化
│   │   ├── model.go / store.go
│   │   └── store_test.go / store_bench_test.go
│   └── ansi/color.go           # TTY 检测 + ANSI 着色
└── README.md
```
