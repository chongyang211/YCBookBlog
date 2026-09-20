#!/usr/bin/env bash
# Mini-KV 统一运行器
# 用法: ./run.sh [repl|test|bench|demo|cme]
cd "$(dirname "$0")"

if [ ! -d out ]; then
    echo "请先编译: ./build.sh"
    exit 1
fi

case "${1:-repl}" in
    repl)
        # 交互式 REPL（支持管道喂命令，如: printf "SET a 1\nGET a\nquit\n" | ./run.sh repl）
        java -cp out com.kv.cli.Main ;;
    test)
        # 内置单测框架：10 个测试
        java -cp out com.kv.test.RunAll ;;
    bench)
        # 性能压测：内存 QPS + AOF 三档刷盘
        java -cp out com.kv.test.Benchmark ;;
    demo)
        # Value.parse 类型推断演示
        java -cp out com.kv.store.ValueDemo ;;
    cme)
        # 造 BUG #4 现场：100 线程并发 HashMap → CME + 数据丢失
        java -cp out com.kv.test.ConcurrencyDemo ;;
    *)
        echo "用法: ./run.sh [repl|test|bench|demo|cme]"
        exit 1 ;;
esac
