# OrderForge · 订单锻造厂（六大设计原则毕业设计）

> 与专栏《常见设计原则》综合案例配套的教学工程。
>
> **从 0 写一个"能跑的烂订单系统"（上帝类 + 3 个真实 bug），用六大原则做六次手术，用度量报告证明"治好了"。** V0 是生铁，终局是锻件。

## 环境要求

- **JDK 17+**（`java --version` 确认）
- 无需 Maven / Gradle / JUnit —— 纯 JDK + 自建 testkit

## 快速开始

```bash
./build.sh            # 编译全部
./run.sh v0           # V0 上帝类：亲眼看见 3 个真实 bug
./run.sh metrics      # 圈复杂度 + 坏味道体检报告
./run.sh v1 .. v4     # 四次手术的 demo + 单测
./run.sh report       # 终局验收：度量前后对比 + 三改实验
./run.sh test-all     # 29 个测试全量回归
```

## 版本演进路线

| 版本 | 包名 | 手术 | 治好的病 | 对应博客 |
|---|---|---|---|---|
| **V0** | `v0god` | ——（亲手写出来） | ——（先看见病） | 01/02/08 |
| **度量** | `metrics` | 圈复杂度 + 坏味道扫描 | 诊断报告 | 08 |
| **V1** | `v1srp` | 拆类：六件事六个类 | 库存凭空消失（BUG 2 止血） | 02 |
| **V2** | `v2ocp` | 折扣 if-else → 策略链 | 多收 125 元（BUG 1） | 03 |
| **V3** | `v3lspisp` | 支付契约 + 角色接口 | 契约外异常 / 胖接口空壳 | 04/05 |
| **V4** | `v4diplod` | 依赖注入 + 门面 | 超限炸中途（BUG 2）/ 通知 NPE（BUG 3） | 06/07 |
| **终局** | `report` | 度量对比 + 三改实验 | 评估报告 | 08 |

## 目录结构

```
OrderForge/
├── README.md · build.sh · run.sh
└── src/com/yc/orderforge/
    ├── shared/          Money 值对象 + testkit
    ├── domain/          Order / Customer / Address / City
    ├── v0god/           上帝类 OrderManager + 3 bug 现场
    ├── metrics/         圈复杂度 + 坏味道扫描器（自建工具）
    ├── v1srp/           SRP：六个服务 + 协调者 + 补偿
    ├── v2ocp/           OCP：DiscountStrategy 策略链
    ├── v3lspisp/        LSP：支付契约测试｜ISP：角色接口
    ├── v4diplod/        DIP：MessageSender/OrderRepository 注入｜LOD：门面 + Tell Don't Ask
    └── report/          终局验收报告
```

## 每版验收命令

| 版本 | 命令 | 期望 |
|---|---|---|
| V0 | `./run.sh v0` | 订单 2 多收 ¥125 / 订单 3 库存消失 / 订单 4 通知 NPE |
| 度量 | `./run.sh metrics` | placeOrder CC=14（>10 必须重构）+ 坏味道 4 条 |
| V1 | `./run.sh v1` | 支付失败库存自动回滚；6 个服务可独立单测 |
| V2 | `./run.sh v2` | GOLD 1000 = ¥675（修复）；加折扣零改动核心类 |
| V3 | `./run.sh v3` | 契约测试抓获违约标本；买家接口 2 方法 0 空壳 |
| V4 | `./run.sh v4` | 3 bug 全修；换渠道/换仓储 0 行修改 |
| 终局 | `./run.sh report` | CC 14→2、坏味道 4→0、三改实验全过 |

## 学习建议

- **按顺序** V0 → metrics → V1~V4：每次手术只治一种病，别跳；
- **先自己改**：看 V0 后先动手拆，卡住再对照 V1——"先撞墙，再看答案"；
- **对照博客**：每个手术对应一篇原则（见上表），术前重读那篇效果最好。
