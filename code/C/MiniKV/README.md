# MiniKV —— C语言 迷你KV存储引擎

《C语言入门精通》综合案例 06（毕业设计）的配套源码：**迷你 Redis**——多类型 Value、手写哈希表、命令模式、AOF 持久化、TTL 后台清理、多线程安全，约 1200 行零依赖 C11。

教程地址：`packages/website/11.CodeX/01.C语言入门精通/02.综合案例/06.迷你KV存储引擎器.md`

## 编译运行

```bash
make            # 编译 mini_kv + 5 个测试
make test       # 跑单元测试（value / store / aof）
./mini_kv       # 交互式 REPL
./test_race     # 数据竞争造 BUG 实验
./stress        # 8 线程 160 万次混合读写压测
```

日志级别：`make CFLAGS+="-DKV_LOG_LEVEL=2"` 只留 WARN+ERROR。

## 一轮完整会话

```text
> SET name zhangsan
OK
> SET age 30
OK
> SET pi 3.14
OK
> GET pi
(double) 3.14
> KEYS *
1) "name"
2) "age"
3) "pi"
> EXPIRE name 5
OK
> TTL name
(integer) 4
（等 6 秒——后台 purger 线程已清理）
> GET name
(nil)
> DEL pi
(integer) 1
> EXIT
bye.
```

## AOF 持久化验证（kill -9 也不丢）

```text
$ ./mini_kv
> SET project minikv → OK
> ^C                      # 或 kill -9 强杀

$ ./mini_kv               # 重启
[..] [INFO] [src/main.c:.. main] replayed 1 keys
> GET project → "minikv"
```

## 模块结构（与教程章节对应）

| 文件 | 教程阶段 | 职责 |
|---|---|---|
| `include/mkv/errors.h` + `src/errors.c` | §07 | 错误码 + 出参上下文（C 的异常体系）|
| `include/mkv/value.h` + `src/value.c` | §03 | tagged union Value + 类型推断 |
| `include/mkv/entry.h` | §04 | Entry + TTL（MONOTONIC 时钟）|
| `include/mkv/log.h` | §09 | KV_LOG 宏（do-while(0) + `__FILE__`）|
| `include/mkv/hashmap.h` + `src/hashmap.c` | §06 | **手写 FNV-1a 哈希表**（链地址 + rehash + `**pp` 删节点）|
| `include/mkv/store.h` + `src/store.c` | §06/§10 | Store 门面：rwlock 读写分类锁 |
| `include/mkv/command.h` + `src/command.c` | §05 | 命令模式（基类嵌入 = C 的虚函数）|
| `include/mkv/aof.h` + `src/aof.c` | §08 | AOF 追加日志 + 宽松/严格重放 |
| `include/mkv/server.h` + `src/server.c` | §11 | TTL 后台线程（atomic_bool 协作停止）|
| `src/main.c` | §12 | REPL + 逆序收尾（server→aof→store）|

模块依赖严格单向：`main → server/command → store → hashmap → entry → value → errors/log`。

## 距生产 Redis 的差距（教程 §13.3）

本地 REPL 无网络；仅 4 种值类型（List/Hash 留作挑战一）；AOF 无 rewrite（挑战二）；无 fsync 调优、主从与集群。
