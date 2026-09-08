# BankSystem — Java 银行账户管理系统

《Java 入门精通》综合案例的**第二关·OOP 大考**。从上一案例"学生成绩"的"三个并行数组 + 静态方法"跃迁到工业级 OOP：**class 封装 + 抽象类 + 多态三态 + 自定义异常体系 + CSV 持久化 + 分包架构**。

## 功能（8 项）

| 菜单项 | 功能 | 核心 |
|--------|------|------|
| 1 开户 | 选择类型（普通/VIP/储蓄） | 多态创建 |
| 2 销户 | 按账号删除 | 末尾填洞法 |
| 3 存款 | 余额累加 | 异常校验 |
| 4 取款 | 三态差异化规则 | **多态分派** |
| 5 转账 | 双方原子变更 | 补偿事务 |
| 6 查询 | 显示详细信息（含子类字段） | instanceof |
| 7 月结息 | 遍历计息 | **多态循环** |
| 0 退出 | 保存后退出 | CSV 持久化 |

## 三类账户业务规则（多态三态）

| 类型 | 标签 | 取款规则 | 月利率 |
|------|------|---------|--------|
| NormalAccount 普通 | `N` | 不允许透支 | 0.1% |
| VipAccount VIP | `V` | 允许透支至 -creditLimit（默认 1000） | 0.5% |
| SavingsAccount 储蓄 | `S` | 锁定期内取款扣 1% 违约金 | 0.3% |

## 项目结构（5 包分层）

```text
src/com/bank/
├── entity/                 # 实体层（数据 + 行为）
│   ├── Account.java        # abstract 基类
│   ├── NormalAccount.java
│   ├── VipAccount.java
│   └── SavingsAccount.java
├── exception/              # 异常层
│   ├── BankException.java  # 基类
│   ├── InsufficientFundsException.java
│   ├── AccountNotFoundException.java
│   └── InvalidAmountException.java
├── persist/
│   └── Persistable.java    # 持久化接口 + fromCsv 工厂
├── dao/
│   └── AccountDao.java     # CSV 文件读写
├── service/
│   └── BankService.java    # 业务编排
└── cli/
    ├── Menu.java           # 主菜单（含 main）
    └── AccountTest.java    # 端到端自测
```

## 编译运行

```bash
cd BankSystem

# 编译（输出到 out/）
javac -d out -encoding UTF-8 $(find src -name "*.java")

# 运行主程序
java -cp out com.bank.cli.Menu

# 运行自测（7 条断言）
java -cp out com.bank.cli.AccountTest

# 或一键编译 + 运行
./build.sh
```

## 核心知识点

- **class 封装**：字段 `private` + getter，`balance` 只读（无 setter）
- **构造重载 + `this()`**：3 个构造链式复用，真正赋值只有一份
- **抽象类 + 多态三态**：`abstract withdraw()/calcInterest()` 强制子类实现，同一段循环三态各显神通
- **异常体系**：`BankException` 基类 + 3 个派生异常带专属字段，替代 `-1` 哨兵
- **接口 `Persistable`**：`toCsv` 抽象 + `toJson` default + `fromCsv` static 工厂
- **CSV 持久化**：`try-with-resources` + 显式 UTF-8 + 单行容错跳过
- **补偿事务**：转账"先扣再加 + 失败回滚"
- **JDK 17 特性**：`switch` 表达式、`instanceof` 模式匹配、`yield`

## 关键陷阱（新手经典坑）

1. **错误码反例**：`withdraw` 返 `boolean` 让调用方丢失"为什么失败"
2. **向下转型 BUG**：父类引用调子类专属方法编译失败 → `instanceof` 模式匹配
3. **不关流数据丢失**：`BufferedWriter` 不 close 直接退出 → 文件空 → `try-with-resources`
