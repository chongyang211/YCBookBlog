# Mini-KV · Java 迷你 KV 存储引擎

> 对标 Redis 最小子集的单机内存 KV 引擎（JDK 17，零第三方依赖）。
> 配套教学文档：`packages/website/09.CodeX/03.Java入门精通/02.综合案例/06.迷你KV存储引擎器.md`
> （11 个阶段 / 5 次会话 / 9 处灵魂三问 / 5 个造 BUG 现场）

## 三大手造框架（本项目的灵魂）

| 手造 | 对应真实框架 | 位置 |
|---|---|---|
| `@Command` 注解 + 反射扫包注册 | Spring `@Component` 扫描 | `command/CommandRegistry.java` |
| AOF 追加写 + 宽松重放 | Redis AOF / WAL 日志 | `aof/AofWriter.java` + `aof/AofReplayer.java` |
| `@Test` + 反射 Runner + Assert | JUnit 5 | `test/TestRunner.java` |

## 快速开始

```bash
./build.sh          # 纯 javac 编译，无网络依赖
./run.sh repl       # 交互式 REPL（默认 EVERY_SECOND 刷盘）
./run.sh test       # 内置单测框架：10 个测试
./run.sh bench      # 性能压测：内存 QPS + AOF 三档刷盘
./run.sh demo       # Value.parse 类型推断演示
./run.sh cme        # 造 BUG #4 现场：100 线程并发 HashMap
```

REPL 支持：`SET key value` / `GET key` / `DEL key` / `EXPIRE key seconds` /
`KEYS` / `PING` / `DEMO n`（批量写，供压测与 round-trip 验证）。

```text
kv> SET name zs
OK
kv> GET name
"zs"
kv> KEYS
1) "name"
kv> EXPIRE name 5
(integer) 1
kv> FOO
(error) ERR unknown command 'FOO'
kv> quit
bye.
```

**AOF 重启恢复（round-trip）**：退出后重启，数据自动恢复——

```text
[INFO] AOF 重放: 共 4 行, 成功 4, 跳过 0
kv> GET k50
"v50"
```

## 项目结构

```
src/main/java/com/kv/
├── cli/Main.java               # REPL 入口：扫描注册 → AOF 重放 → 刷盘线程 → TTL → REPL
├── store/                      # 数据层
│   ├── Value.java              #   sealed interface + 6 record + parse 类型推断
│   ├── NullValue ~ ListValue   #   6 种值类型（防御性不可变拷贝）
│   ├── Entry.java              #   record(value, expireAtMs, version)
│   ├── Store.java              #   ConcurrentHashMap + compute 原子 + SkipListMap 过期索引
│   ├── TtlCleaner.java         #   守护线程主动清理（volatile + interrupt + join）
│   └── ValueDemo.java          #   类型推断演示入口
├── command/                    # 命令层
│   ├── Command.java            #   抽象基类（模板方法：name/execute/isWrite/aofLine）
│   ├── SetCmd ~ DemoCmd        #   7 个命令（加新命令 = 新建一个类标 @Command）
│   ├── CommandFactory.java     #   阶段④手写注册（教学对照）
│   └── CommandRegistry.java    #   阶段⑤反射扫包注册（Spring 雏形，终态）
├── annotation/Command.java     # @Command 注解（Retention 必须 RUNTIME）
├── aof/                        # 持久化层
│   ├── AofWriter.java          #   APPEND + write/flush/fsync 三层 + 三档策略
│   ├── AofReplayer.java        #   严格模式（教学）/ 宽松模式（生产，容忍半行损坏）
│   └── FlushPolicy.java        #   EVERY_WRITE / EVERY_SECOND / NEVER
├── exception/                  # KvException 基类 + 5 派生（unchecked，Redis 风格 hint）
└── log/Log.java                # 自写日志门面（4 级别，-Dlog.level 切换）

src/test/java/com/kv/test/
├── Test.java / TestRunner.java / Assert.java   # 自写单测框架（JUnit 雏形）
├── AllTests.java               # 10 个单测（类型/Store/TTL/注册/AOF/并发）
├── RunAll.java                 # 一键全跑
├── ConcurrencyDemo.java        # 造 BUG #4：100 线程并发 HashMap → CME + 丢数据
└── Benchmark.java              # 压测：内存 QPS + AOF 三档刷盘对比
```

## 11 个阶段 → 代码映射

| 阶段 | 内容 | 落点 |
|---|---|---|
| ① 最小 REPL | Scanner + enum + switch | `cli/Main.java`（演进终态） |
| ② Value 类型 | sealed interface + 6 record | `store/Value.java` |
| ③ Entry + Store | HashMap 起步 | `store/Entry.java` |
| ④ 命令模式 | 抽象基类 + 5 派生 + Factory | `command/` |
| ⑤ 注解反射注册 | @Command + 扫包（Spring 雏形） | `command/CommandRegistry.java` |
| ⑥ 异常体系 | KvException + 5 派生 + 全局路由 | `exception/` |
| ⑦ AOF 持久化 | Writer/Replayer/三档策略/半行容错 | `aof/` |
| ⑧ 日志门面 | 4 级别 + 系统属性切换 | `log/Log.java` |
| ⑨ 并发安全 | HashMap→CHM + compute + CAS 删除 | `store/Store.java` |
| ⑩ TTL 守护线程 | 双删策略 + SkipListMap 索引 | `store/TtlCleaner.java` |
| ⑪ 内置单测 | @Test + Runner + Assert | `test/` |

## 5 个造 BUG 现场（均可运行复现）

| # | BUG | 复现方式 |
|---|---|---|
| 1 | if-else 大爆炸 → 命令模式 | 教学文档 §06.1（代码里 `CommandFactory` 即过渡形态） |
| 2 | 反射 setAccessible / 无参构造 / RUNTIME 三坑 | `CommandRegistry.tryRegister` 注释（含精确报错） |
| 3 | AOF 半行损坏 | `./run.sh test` 的 `testAofCorrupted`（GARBAGE 行跳过 + 计数） |
| 4 | 100 线程并发 HashMap → CME + 丢数据 | `./run.sh cme` |
| 5 | 守护线程无 volatile 停不下来 | `TtlCleaner` 注释（正确版：volatile + interrupt + join） |

## 验证结果（本机实测）

```text
$ ./run.sh test
==> 共 10 / 通过 10 / 失败 0

$ ./run.sh bench
单线程 10 万次 SET:           43 ms  ≈ 2,325,581 QPS
100 线程 100 万次 SET(CHM):   271 ms  ≈ 3,690,036 QPS（size=1000000）
AOF EVERY_WRITE  1 万条:   42641 ms  ≈ 234 QPS     ← fsync 的真实代价（macOS）
AOF EVERY_SECOND 1 万条:      57 ms  ≈ 175,438 QPS
AOF NEVER        1 万条:       6 ms  ≈ 1,666,666 QPS

$ # AOF round-trip + 半行损坏
[INFO] AOF 重放: 共 6 行, 成功 5, 跳过 1   ← 坏行跳过，数据完整恢复
```

## 可配参数

| 参数 | 默认 | 说明 |
|---|---|---|
| `-Dkv.aof.file` | `appendonly.aof` | AOF 文件路径 |
| `-Dkv.aof.policy` | `EVERY_SECOND` | `EVERY_WRITE` / `EVERY_SECOND` / `NEVER` |
| `-Dlog.level` | `INFO` | `DEBUG` 可看命令注册详情 |

例：`java -Dlog.level=DEBUG -Dkv.aof.policy=EVERY_WRITE -cp out com.kv.cli.Main`

## 延伸挑战（见教学文档 §17.2）

- **挑战 A**：LIST 类型（LPUSH/RPUSH/LPOP/RPOP/LRANGE）
- **挑战 B**：BIO 服务化（ServerSocket + 线程池，telnet 6390 直连）
- **挑战 C**：NIO + Selector（单线程处理万级连接 → 读懂 Netty 的前置）
