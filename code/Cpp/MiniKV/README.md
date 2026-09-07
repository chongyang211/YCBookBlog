# MiniKV — 迷你 KV 存储引擎

一个用现代 C++（C++17/20）从零实现的迷你 Key-Value 存储引擎，支持**多类型值、命令模式、AOF 持久化、TTL 过期、多线程并发安全**，被称为"迷你 Redis"。它是《C++ 入门到精通》卷一综合案例的**毕业设计**，用一个项目串起从基础语法到 `std::variant`、`std::jthread`、宏等 17 章知识点。

## 特性一览

- **8 个命令**：`SET` / `GET` / `DEL` / `EXPIRE` / `TTL` / `KEYS` / `SAVE` / `EXIT`
- **多类型值**：`std::variant` 统一表达 `Null / bool / int64_t / double / string`
- **命令模式**：加新命令只需"新增一个派生类 + 工厂加一行"，主流程零改动
- **AOF 持久化**：写命令追加落盘，启动自动重放，进程重启数据不丢
- **TTL 过期**：惰性删除 + 后台线程主动清理双机制
- **并发安全**：`std::shared_mutex` 读写锁，读多写少场景吞吐更高
- **工程化**：异常体系、日志宏、RAII、多文件分层结构

## 目录结构

```text
MiniKV/
├── CMakeLists.txt            # 构建脚本（C++20 + Threads）
├── README.md                 # 本文件
├── include/mkv/              # 公共头文件（按职责分模块）
│   ├── errors.hpp            # 异常体系：KvError 树
│   ├── value.hpp             # Value 类型 + 类型推断 + 格式化
│   ├── entry.hpp             # Entry / EntryPtr / IndexMap + TTL
│   ├── log.hpp               # KV_LOG 宏 + LogLevel + nowStr
│   ├── store.hpp             # Store：unordered_map + shared_mutex
│   ├── command.hpp           # 命令模式：抽象基类 + 派生类 + 工厂
│   ├── aof.hpp               # AofWriter / replayAof / ReplayReport
│   └── server.hpp            # Server：jthread 后台清理线程
├── src/                      # 实现文件（与头文件一一对应）
│   ├── value.cpp
│   ├── entry.cpp
│   ├── store.cpp
│   ├── command.cpp
│   ├── aof.cpp
│   ├── server.cpp
│   └── main.cpp              # REPL 主入口
├── tests/                    # 测试与压测
│   ├── test_value.cpp        # Value 类型推断
│   ├── test_store.cpp        # Store 七个核心方法
│   ├── test_aof.cpp          # AOF 写盘 + 重放 + 损坏容错
│   ├── test_race.cpp         # 数据竞争现场（裸变量演示）
│   └── stress.cpp            # 多线程压测
└── data/                     # 运行时自动创建，存放 aof.log
```

## 类职责说明

### 数据层

| 类 / 类型 | 文件 | 作用 |
|-----------|------|------|
| `Value` | `value.hpp` | 用 `std::variant<Null, bool, int64_t, double, string>` 表达"一个 key 能存多种类型的值"，并提供 `isInt` / `isDouble` 等类型查询 |
| `valueFromToken` | `value.hpp` | 把字符串 token 自动推断成对应类型：`"42"` → int、`"3.14"` → double、`"true"` → bool、其余 → string |
| `formatValue` | `value.hpp` | 把 `Value` 转成 Redis 风格的可读字符串：`(integer) 42`、`(double) 3.14`、`"hello"`、`(nil)` |
| `Entry` | `entry.hpp` | 单条键值对：`value` + 过期时间 `expireAt`（`std::optional`，`nullopt` 表永久）+ 创建时间 + 版本号 `version`，并提供 `isExpired()` 判断 |
| `EntryPtr` / `IndexMap` | `entry.hpp` | 类型别名：`shared_ptr<Entry>` 与 `unordered_map<string, EntryPtr>`，用 `shared_ptr` 保证"正在被使用的 Entry"不会被后台清理线程误析构 |

### 存储与并发层

| 类 | 文件 | 作用 |
|----|------|------|
| `Store` | `store.hpp` | 核心数据仓库，内部持有一个 `IndexMap` 和一把 `shared_mutex`。提供 `set / get / del / expire / ttl / keys / size / purgeExpired` 八个方法，读操作用共享锁、写操作用独占锁，是并发安全的唯一入口 |

### 命令层（命令模式）

| 类 | 文件 | 作用 |
|----|------|------|
| `Command` | `command.hpp` | 命令抽象基类，定义四条纯虚接口：`name()`（命令名）、`isWrite()`（是否写盘）、`execute(Store&)`（执行）、`toAofLine()`（序列化成 AOF 行） |
| `SetCmd` / `GetCmd` / `DelCmd` / `ExpireCmd` / `TtlCmd` / `KeysCmd` / `SaveCmd` | `command.hpp` | 七个具体命令，每个类内聚一种命令的参数校验、执行逻辑、AOF 序列化 |
| `makeCommand` | `command.hpp` | 工厂函数：把切好的 tokens 组装成对应的 `unique_ptr<Command>`，参数个数校验、类型转换都在这里集中处理 |
| `parseCmdType` / `tokenize` | `command.hpp` | 命令名 → 强类型枚举（大小写不敏感）；一行字符串 → `vector<string_view>`（零拷贝切词） |

### 持久化层

| 类 / 函数 | 文件 | 作用 |
|-----------|------|------|
| `AofWriter` | `aof.hpp` | AOF 追加写日志，RAII 管理文件句柄（析构自动关文件、禁拷贝）。构造时自动建父目录、以 `app + binary` 模式打开 |
| `replayAof` | `aof.hpp` | 启动时重放 AOF 文件恢复内存状态，复用同一套命令解析管道（`tokenize → makeCommand → execute`），支持严格 / 宽松两种损坏处理策略 |
| `ReplayReport` | `aof.hpp` | 重放结果统计：成功重放行数、跳过坏行数、总行数 |

### 服务与系统层

| 类 | 文件 | 作用 |
|----|------|------|
| `Server` | `server.hpp` | 服务外壳，持有后台 TTL 清理线程（`std::jthread`），每 100ms 扫描一次并清理过期 key；析构时自动 `request_stop + join`，优雅停止 |
| `KvError` 异常树 | `errors.hpp` | 统一异常体系：根类 `KvError`，派生 `CmdSyntaxError`（语法错误）、`TypeError`（类型不匹配）、`IoError`（IO 失败）、`AofCorrupted`（AOF 坏行，带行号） |
| `KV_LOG_*` 宏 | `log.hpp` | 工程级日志：带时间戳、级别、`__FILE__:__LINE__`、函数名；用 `do-while(0)` 防御 else 错配；通过 `KV_LOG_LEVEL` 条件编译控制开关 |

## 快速开始

### 环境要求

- 编译器支持 **C++20**（`std::jthread` 需要）：GCC ≥ 10 / Clang ≥ 12 / AppleClang 较新版本 / MSVC ≥ 19.30
- CMake ≥ 3.16

### 编译

```bash
cd MiniKV
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

> 想编译成 Debug 版把 `Release` 换成 `Debug` 即可。

### 运行

```bash
./build/MiniKV
```

进入 REPL 后，在 `>` 提示符下输入命令：

```text
MiniKV v1.0 - type EXIT to quit.
> SET name zhangsan
OK
> SET age 30
OK
> GET name
"zhangsan"
> GET age
(integer) 30
> KEYS *
1) "age"
2) "name"
> EXPIRE name 60
OK
> TTL name
(integer) 58
> DEL age
(integer) 1
> EXIT
bye.
```

### 运行测试

```bash
./build/test_value    # Value 类型推断
./build/test_store    # Store 七个核心方法
./build/test_aof      # AOF 写盘 + 重放 + 损坏容错
./build/test_race     # 数据竞争现场（裸变量演示）
./build/stress        # 8 线程 × 10 万次混合读写压测
```

## 命令清单

| 命令 | 语法 | 说明 | 示例输出 |
|------|------|------|----------|
| `SET` | `SET key value` | 写入键值，自动推断类型，覆盖时清 TTL | `OK` |
| `GET` | `GET key` | 读取键值，不存在/已过期返回 nil | `"zhangsan"` / `(nil)` |
| `DEL` | `DEL key` | 删除键，返回删除个数 | `(integer) 1` |
| `EXPIRE` | `EXPIRE key seconds` | 设置过期时间（秒） | `OK` / `(integer) 0` |
| `TTL` | `TTL key` | 查剩余秒数，-2 不存在、-1 永久 | `(integer) 58` |
| `KEYS` | `KEYS pattern` | 列出所有 key | `1) "name"` |
| `SAVE` | `SAVE` | 强制刷盘 | `OK` |
| `EXIT` | `EXIT` | 退出程序 | `bye.` |

支持的值类型：字符串、整数、浮点数、布尔（`true` / `false`）、空（`nil`）。

## 架构与数据流

依赖方向**严格单向**：`main → server / command → store → entry → value`，`aof` 同时依赖 `store` 和 `command`，`errors` / `log` 是叶子模块。

```text
            ┌─────────────┐
            │  main.cpp   │  REPL 主循环
            └──────┬──────┘
                   │
         ┌─────────┴─────────┐
         ▼                   ▼
   ┌───────────┐       ┌───────────┐
   │ server.h  │       │ command.h │
   └─────┬─────┘       └─────┬─────┘
         │                   │
         └─────────┬─────────┘
                   ▼
              ┌──────────┐
              │ store.h  │  shared_mutex + unordered_map
              └────┬─────┘
                   ▼
              ┌──────────┐         ┌──────────┐
              │ entry.h  │ ───────▶│ value.h  │
              └──────────┘         └────┬─────┘
                                        ▼
                                   ┌──────────┐    ┌──────────┐
                                   │ errors.h │    │  log.h   │
                                   └──────────┘    └──────────┘
```

一次请求的完整链路：**读入一行 → `tokenize` 切词 → `makeCommand` 组装命令对象 → `cmd->execute(store)` 多态执行 → 写命令追加 AOF 落盘 → 返回结果**。启动时则用 `replayAof` 走同一条命令解析管道恢复数据。

## 关键设计取舍

- **`shared_ptr<Entry>` 而非 `unique_ptr`**：后台清理线程扫描、AOF 写盘都要"借用" Entry，引用计数保证使用中的对象不会被提前析构。
- **`shared_mutex` 而非 `mutex`**：KV 负载读多写少（GET:SET 通常 10:1），共享读锁让 GET 并发，压测可到百万级 ops/s。
- **命令模式而非 if-else**：加新命令 = 新增一个派生类 + 工厂加一行，主流程零修改，符合开闭原则。
- **AOF 而非快照**：追加日志实现简单、天然支持崩溃恢复，坏行默认宽松跳过（记日志继续），避免服务因个别坏数据无法启动。
- **`steady_clock` 而非 `system_clock`**：单调时钟不受用户改系统时间影响，是 TTL 的唯一正确选择。
