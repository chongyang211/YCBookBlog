# FlowKit 审批引擎

《巧学设计模式》最终关·毕业设计：**把 23 种设计模式装进同一个迷你审批引擎**。
配套文章见 `packages/website/06.编程/03.巧学设计模式/25.案例FlowKit审批引擎.md`。

## 编译与运行

```bash
cd code/Programming/FlowKit
javac -d out $(find src -name "*.java")
java -cp out flowkit.App
```

要求 JDK 17+（record / switch 表达式 / instanceof 模式匹配）。

## REPL 命令

| 命令 | 说明 | 演示的模式 |
|------|------|-----------|
| `submit refund 4800 "键盘进水"` | 提交售后退款工单（金额单位：分） | 建造者、工厂、单例、外观、中介者 |
| `status` | 查看工单状态与审批链进度 | 状态 |
| `approve zhangsan` | 审批通过（当前节点） | 职责链、策略、模板方法、命令、动态代理 |
| `reject lisi 金额存疑` | 驳回 | 状态、命令、中介者 |
| `addsign wangwu` | 加签：当前节点前插一个审批人 | 职责链（链手术）、命令 |
| `withdraw` | 撤回（回到草稿，四方联动） | 状态、中介者 |
| `undo` | 撤销上一步审批动作 | 命令 + 备忘录 |
| `attach 发票.pdf` | 附加凭证 | 备忘录（深拷贝） |
| `expr amount > 5000 AND dept == '研发'` | 测试条件 DSL | 解释器（AST 缓存） |
| `notify` | 手动触发多渠道通知 | 适配器 + 桥接 + 观察者 |
| `stats` | 财务/审计/耗时三视角统计 | 访问者、组合 |
| `history` | 审批全史时间线 | 迭代器（双源归并） |
| `replay` | 从命令日志恢复（崩溃恢复） | 命令日志（AOF） |
| `flow refund` | 展示流程定义与图标池 | 享元、原型、懒加载代理 |
| `quit` | 退出（live 事件归档到 `data/history/`） | 迭代器（归档源） |

## 建议体验路径（对应文章五次会话）

```text
submit refund 4800 "键盘进水"     # 工厂铺链 → 表单锁定 → 钉钉待办
approve zhangsan                 # 状态机 → 模板方法 → 职责链 → 审计代理
addsign wangwu                   # 链上动态插节点
approve wangwu                   # 加签人通过
approve lisi                     # 主管通过 → 条件路由(amount>5000 → false) → 终审
status / stats / history         # 状态 / 三视角统计 / 时间线
replay                           # 重启后从 data/action.log 恢复现场
```

会签流程演示：`submit leave 4800 "年假三天"` 后 `approve zhangsan`、`approve hr1`、`approve hr2`。

## 23 模式落位速查

| 类别 | 模式 → 落位 |
|------|------------|
| 创建型 5 | 单例→`ProcessRegistry`；工厂方法→`NodeFactories`；建造者→`ProcessDef.Builder`/`Ticket.Builder`；原型→`ProcessDef.clone()`；享元→`ui/IconPool` |
| 结构型 7 | 代理→`LazyProcessProxy`；动态代理→`audit/AuditProxy`；适配器→`notify/*Adapter`；桥接→`Notification`×`Channel`；装饰→`TimeoutDecorator`/`CcDecorator`；外观→`WorkflowFacade`；组合→`task/*` |
| 行为型 11 | 模板→`AbstractNodeExecutor`；策略→`ApproverFinder`；观察者→`EventBus`/`AsyncEventBus`；迭代器→`TimelineIterator`；职责链→`ApproverNode`；命令→`action/*Command`；状态→`domain/state/*`；备忘录→`TicketMemento`；中介者→`FlowMediator`；访问者→`stats/*Visitor`；解释器→`rule/*` |

## 目录结构

```text
FlowKit/
├── src/flowkit/
│   ├── App.java                # REPL 入口（纯壳）
│   ├── domain/                 # 工单 + 状态机（状态模式）
│   ├── org/                    # 组织架构桩（策略的数据源）
│   ├── engine/                 # 流程引擎核心（职责链/策略/工厂/建造者/原型/装饰/模板/代理/单例）
│   ├── task/                   # 任务树（组合模式）
│   ├── action/                 # 命令 + 备忘录 + 调度器（Caretaker）
│   ├── rule/                   # 条件 DSL（解释器 + AST 缓存）
│   ├── notify/                 # 多渠道通知（适配器 + 桥接 + 观察者）
│   ├── audit/                  # 命令日志（AOF）+ 动态代理审计
│   ├── history/                # 历史时间线（迭代器双源归并）
│   ├── stats/                  # 三视角统计（访问者）
│   ├── ui/                     # 图标池（享元）
│   └── facade/                 # 外观 + 中介者 + 四方服务桩
├── data/                       # 运行时生成：action.log + history/yyyy-MM.log
└── README.md
```

## 故意不用模式的三个地方

1. 工单存储 = 内存 Map（只有一个实现，抽象没有第二个实现者就是过度设计）；
2. OrgService/RoleService = 普通类（"恰好只建了一个" != 单例，全项目唯一单例只有 ProcessRegistry）;
3. REPL 分发 = switch（审批动作需要撤销和重放所以命令化；REPL 命令不需要，模式只有成本）。
