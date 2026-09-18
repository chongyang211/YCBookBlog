# PayFlow · 电商订单结算引擎（面向对象设计毕业设计）

> 与专栏《面向对象设计》第 14 篇「综合案例·电商订单结算引擎」配套的教学工程。
>
> 从 V0 屎山（双 11 雪崩事故代码原景重现）演化到 V7 DDD 工程级，8 个版本、每版一个包、每版可运行、每版有测试。

## 环境要求

- **JDK 17+**（`java -version` 确认）
- 无需 Maven / Gradle / JUnit —— 纯 JDK + 自建 5 行断言工具
- macOS / Linux / Windows（WSL）皆可

## 快速开始

```bash
# 编译全部版本
./build.sh

# 运行 V0 屎山演示（双 11 事故代码，含 1 个真实 bug）
./run.sh v0

# 运行 V1 演示 + 测试
./run.sh v1

# 一键跑全部版本的测试
./run.sh test-all
```

## 版本演进路线

| 版本 | 包名 | 一句话目标 | 对应博客 |
|---|---|---|---|
| **V0** | `v0shipan` | 看见病（1284 行雪崩现场） | 01/08 |
| **V1** | `v1feature` | 数据有守卫（Wallet 封装 + VipPolicy 多态） | 02 |
| **V2** | `v2structure` | 依赖可替换（通知双层 + 支付渠道 + 装饰器） | 03/04 |
| **V3** | `v3composition` | 能力可装配（虚拟商品不该继承 ship） | 05 |
| **V4** | `v4solid` | 变化有扩展点（PromotionStep SPI） | 06/07 |
| **V5** | `v5refactor` | 屎山安全救活（特征化测试挖出 bug） | 08/09 |
| **V6** | `v6testable` | 测得真（Clock 注入 + 秒杀折扣测试） | 10 |
| **V7** | `v7ddd` | 业务对齐（五上下文 + 领域事件） | 11/12 |

## 目录结构

```
PaySystem/
├── README.md
├── build.sh                    # 编译全部版本
├── run.sh                      # 运行任意版本 demo/test
├── src/
│   └── com/yc/payflow/
│       ├── shared/             # 通用工具（Money + testkit）
│       ├── v0shipan/           # V0 屎山
│       ├── v1feature/          # V1 封装 + 多态
│       ├── v2structure/        # V2 双层范式 + 装饰器
│       ├── v3composition/      # V3 能力组合
│       ├── v4solid/            # V4 SOLID 五马分尸
│       ├── v5refactor/         # V5 特征化测试 + 12 式
│       ├── v6testable/         # V6 Clock 注入
│       └── v7ddd/              # V7 DDD 终局
└── build/                      # 编译产物（build.sh 自动生成，可 rm）
```

## 每版验收命令一览

| 版本 | 命令 | 期望 |
|---|---|---|
| V0 | `./run.sh v0` | 打印 3 个订单账单，其中订单 2 存在 0.4 元少收（真实 bug） |
| V1 | `./run.sh v1` | 钱包不变量校验通过、VipPolicy 消灭 switch |
| V2 | `./run.sh v2` | 新加邮件通道零改动主流程、支付重试装饰器生效 |
| V3 | `./run.sh v3` | 虚拟商品不再被迫实现 `ship()` |
| V4 | `./run.sh v4` | 新增积分抵扣不改 SettlementEngine 一行 |
| V5 | `./run.sh v5` | 特征化测试锁定 V0 bug、12 式重构后修复 |
| V6 | `./run.sh v6` | 秒杀折扣测试 <10ms（注入 Clock 替代 `new Date()`）|
| V7 | `./run.sh v7` | 领域事件触发多订阅方，结算代码零耦合 |

## 学习建议

- **按顺序**读 V0→V7，每版是上一版的自然演化，跳读会漏关键叙事；
- **动手改代码**：每版都留了"造 BUG → 修复"的现场，先把 bug 复现出来再看修复；
- **对照博客**：每个改动都有博客锚点（见文档 §4.1 的博客案例回收映射表）。
