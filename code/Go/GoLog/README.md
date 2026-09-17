# golog

高性能 Nginx 日志分析流水线 —— Go 综合案例 02，零第三方依赖（仅 stdlib）。

**性能承诺**：流式处理大文件（10 GB 不整读入内存）、零分配解析、内存峰值远低于 50 MB。

## 快速开始

```bash
# 管道方式
cat access.log | go run ./cmd/golog --top 5

# 文件参数方式（等价）
go run ./cmd/golog --top 5 access.log

# 多文件聚合
go run ./cmd/golog access.1.log access.2.log

# 全局安装
go install ./cmd/golog
```

## 用法

```bash
golog [--top N] [--format table|json|csv] [--bucket SECONDS] [files...]
```

| 参数 | 默认 | 说明 |
|---|---|---|
| `--top` | 10 | Top-N 条目数 |
| `--format` | table | 输出格式：table / json / csv |
| `--bucket` | 1 | QPS 时间桶大小（秒） |

无文件参数时自动读 stdin；首个位置参数若是 `stat` / `top-ip` 等子命令名会被忽略（默认即 stat 全量统计行为）。

## 输入格式

标准 Nginx combined log：

```
192.168.1.1 - - [23/May/2026:20:30:12 +0800] "GET /api/v1/user?id=1 HTTP/1.1" 200 1234 "-" "curl/7.81.0"
```

## 设计要点

- **流式处理**：`bufio.Scanner`（64KB 初始 / 1MB 单行上限），每行只在内存中存在一瞬
- **零分配解析**：`bytes.IndexByte` 定界 + 子切片视图（无 make/append），URL 自动去 query string
- **泛型容器**：`Counter[K]`（map 值类型计数）+ `TopK[K]`（最小堆，O(n log k)）
- **Reporter 接口**：table / json / csv 三实现可切换
- **退出码语义化**：0 成功、2 格式错误、3 IO 错误

## 目录结构

```
golog/
├── go.mod
├── cmd/golog/main.go           # 入口
├── internal/
│   ├── parser/                 # 零分配解析
│   │   ├── entry.go / parser.go
│   │   └── parser_test.go / parser_bench_test.go
│   ├── aggregate/              # 泛型容器
│   │   ├── counter.go / topk.go / qps.go
│   │   └── aggregate_test.go
│   ├── pipeline/
│   │   └── run.go              # Reader → Parser → Aggregator
│   │   └── run_test.go
│   └── report/
│       ├── reporter.go / table.go / json.go / csv.go
│       └── report_test.go
└── README.md
```

## 测试与基准

```bash
go test -race ./...
go test -bench=. -benchmem ./internal/parser
# 朴素版 vs 流式版对比：ns/op、B/op、allocs/op 三套数据
```
