# MiniKVStore — 多文件可编译源码包

本目录是《C++ 入门到精通》卷二第 12 章 [`12.迷你KV存储引擎.md`](../../12.迷你KV存储引擎.md) 配套的**可直接编译**多文件版本。
书中正文为方便讲解使用单文件示意，本工程把代码按职责拆成头文件 + 源文件 + 测试。

> 阅读建议：先读书中正文，了解每段代码的"为什么这样写"；再回到本目录读完整工程，体会"工程组织"的价值。

---

## 目录结构

```
12.MiniKVStore/
├── CMakeLists.txt
├── README.md                  ← 本文件
├── include/
│   └── mkv/
│       ├── errors.hpp         ← 异常体系（KvError / CmdSyntaxError / IoError ...）
│       ├── value.hpp          ← Value = variant<Null, bool, int64, double, string>
│       ├── entry.hpp          ← Entry + EntryPtr + IndexMap
│       ├── log.hpp            ← KV_LOG 宏 + LogLevel
│       ├── store.hpp          ← Store 声明（shared_mutex 并发保护）
│       ├── command.hpp        ← Command 抽象基类 + 6 个派生类 + makeCommand 工厂
│       ├── aof.hpp            ← AofWriter + replayAof
│       └── server.hpp         ← Server（jthread 后台 TTL 清理）
├── src/
│   ├── value.cpp
│   ├── log.cpp
│   ├── store.cpp
│   ├── command.cpp
│   ├── aof.cpp
│   ├── server.cpp
│   └── main.cpp               ← 主入口（REPL）
└── tests/
    └── stress.cpp             ← 多线程压测
```

---

## 编译

要求：

- CMake ≥ 3.16
- 支持 C++20 的编译器：GCC ≥ 10、Clang ≥ 12、MSVC ≥ 19.30（VS 2022）
- POSIX 系统下需要 pthread（CMake `find_package(Threads)` 自动处理）

```bash
# 默认 Release + 默认日志级别（Info）
cmake -B build
cmake --build build -j

# 产物
ls build/mini_kv build/mini_kv_stress
```

可选编译参数：

```bash
# 调高日志，看更多 DEBUG
cmake -B build -DKV_LOG_LEVEL=0
cmake --build build -j

# 只保留 Warn / Error
cmake -B build -DKV_LOG_LEVEL=2
```

---

## 运行

### 启动 REPL

```bash
$ ./build/mini_kv
[14:30:01] [INFO ] [.../main.cpp:55 main] replayed 0 keys
[14:30:01] [INFO ] [.../server.cpp:17 operator()] ttl purger thread started
MiniKV v1.0 — type EXIT to quit.
> SET name zhangsan
OK
> SET age 30
OK
> SET pi 3.14
OK
> SET active true
OK
> GET name
"zhangsan"
> GET age
(integer) 30
> GET pi
(double) 3.140000
> GET active
true
> KEYS
1) "name"
2) "age"
3) "pi"
4) "active"
> EXPIRE name 5
OK
> TTL name
(integer) 4
# 等 6 秒
> GET name
(nil)
> DEL pi
(integer) 1
> SAVE
OK
> EXIT
bye.
```

### 验证 AOF 持久化

```bash
$ ./build/mini_kv
> SET project minikv
OK
> SET stars 1024
OK
> ^C                # 强制中断（Ctrl+C）

$ cat data/aof.log
SET project minikv
SET stars 1024

$ ./build/mini_kv
[...] replayed 2 keys
> GET project
"minikv"
> GET stars
(integer) 1024
```

### 跑压力测试

```bash
$ ./build/mini_kv_stress
1600000 ops in 412 ms = 3.88e+06 ops/s
final size = 100
```

---

## 支持的命令

| 命令 | 用法 | 说明 |
|------|------|------|
| `SET`    | `SET key value`        | 设置 key（自动推断类型；会清除旧 TTL） |
| `GET`    | `GET key`              | 读取 key（不存在或已过期返回 `(nil)`） |
| `DEL`    | `DEL key`              | 删除 key，返回删除数（0 / 1） |
| `EXPIRE` | `EXPIRE key seconds`   | 设置过期时间（秒） |
| `TTL`    | `TTL key`              | 剩余秒；`-2` 不存在，`-1` 永久 |
| `KEYS`   | `KEYS` 或 `KEYS *`     | 列出所有未过期 key |
| `SAVE`   | `SAVE`                 | 强制 flush AOF（不写入新行） |
| `EXIT`   | `EXIT` 或 `Ctrl-D`     | 退出 |

值类型自动推断：`true`/`false` -> bool；`nil` -> Null；纯数字 -> int64；带 `.` 能 parse -> double；其他 -> string。

---

## 文件依赖图

```
main.cpp ────┐
             │
             ▼
       ┌──────────┐    ┌──────────┐    ┌──────────┐
       │ command  │───▶│  store   │───▶│  entry   │
       └────┬─────┘    └────┬─────┘    └────┬─────┘
            │               │               │
            ▼               ▼               ▼
       ┌──────────┐    ┌──────────┐    ┌──────────┐
       │  errors  │    │   log    │    │  value   │
       └──────────┘    └──────────┘    └──────────┘

       ┌──────────┐    ┌──────────┐
       │   aof    │───▶│ command  │   （重放走同一条命令解析路径）
       └──────────┘    └──────────┘

       ┌──────────┐    ┌──────────┐
       │  server  │───▶│  store   │   （后台 jthread 调 store.purgeExpired）
       └──────────┘    └──────────┘
```

依赖单向流动，所有头文件都用 `#pragma once`。

---

## 章节对照

| 卷一章节 | 在本工程哪里能看到 |
|---------|-------------------|
| 第 5 章 enum class | `command.hpp` 的 `CmdType` |
| 第 7 章 lambda     | `server.cpp` 的 jthread 入口 lambda |
| 第 8 章 string_view | `command.cpp` 的 `tokenize` |
| 第 9 章 const / 五法则 | `Store` 全部 `= delete`、`Entry` 零自定义成员 |
| 第 10 章 虚函数 vtable | `command.hpp` 的 `Command` 体系 |
| 第 11 章 内存分区 | `Entry`/`Store`/`shared_ptr` 的栈/堆分布 |
| 第 12 章 RAII / 智能指针 | `AofWriter`、`shared_ptr<Entry>` |
| 第 13 章 STL / 流 | `unordered_map<string, EntryPtr>`、`std::ofstream` |
| 第 14 章 异常 / noexcept | `errors.hpp`、`Entry::isExpired` 等的 noexcept |
| 第 15 章 多线程 | `Server` 的 jthread、`Store` 的 shared_mutex |
| 第 16 章 STL 易错点 | `Store::purgeExpired` 的迭代器删除写法 |
| 第 17 章 预处理器 | `log.hpp` 的 `KV_LOG_*` 宏 |
| 第 18 章 C++17/20 | `variant` / `optional` / `filesystem` / `shared_mutex` / `jthread` |

---

## 延伸挑战（书中 §15）

1. **加 List/Hash 复合类型**：把 `Value` 扩成 `variant<..., std::vector<Value>, std::unordered_map<std::string, Value>>`；
2. **AOF rewrite**：当 `aof.log` 大到一定阈值，dump 当前内存为最简命令列表，原子 rename 替换；
3. **网络 socket 接入**：把 REPL 换成 TCP 服务端，用 `epoll`/`kqueue` 处理多客户端。

完成任意一个，就跨入了"卷三：网络与系统编程"的门槛。
