# SchoolManager — Java 校园身份预约系统

《Java 入门精通》综合案例的**第三关·集合框架大考**。从上一案例"银行账户"的 `Account[]` 容量写死 100，跃迁到工业级集合框架：**8 种集合按场景选用 + 多角色多态 + 双索引 + 现代 Java 三剑客 + RFC 4180 CSV 转义**。

## 功能（12 项，三角色）

| 角色 | 功能 |
|------|------|
| 学生 | 浏览房间（容量降序）、提交预约、我的预约、取消预约 |
| 教师 | 发布演讲、Top-K 排行榜、按系别平均分 |
| 管理员 | 审批/驳回预约、增删用户、增删房间、最近浏览(LRU)、系统总览 |

## 8 种集合选型

| 数据表 | 集合 | 理由 |
|--------|------|------|
| 用户表 | `HashMap<String, User>` | 按 ID O(1) 登录 |
| 房间表 | `TreeMap<String, Room>` | 按 ID 字典序输出 |
| 房间按容量排序 | `Room implements Comparable` | 容量降序自然排序 |
| 最近浏览 | `LinkedHashMap`(accessOrder=true) | LRU 缓存 |
| 预约主索引 | `HashMap<String, Reservation>` | 按预约号 O(1) |
| 预约时间索引 | `Map<String, TreeMap<LocalDateTime, Reservation>>` | `subMap` 区间冲突检测 |
| 演讲列表 | `ArrayList<Speech>` | 顺序遍历 |
| Top-K 排行榜 | `PriorityQueue<Speech>`(小顶堆) | O(n log k) 流式 |

## 项目结构（6 包 / 16 类）

```text
src/com/campus/
├── entity/       User(abstract) + Student/Teacher/Admin + Room + Speech + Reservation
├── enums/        Role / RoomType / ReservationStatus
├── repository/   UserRepository(HashMap) + RoomRepository(TreeMap+LRU)
├── service/      ReservationService(双索引) + SpeechService(PriorityQueue)
├── util/         CsvUtil(RFC 4180 转义)
└── cli/          Main(三角色多态菜单)
```

## 编译运行

```bash
cd SchoolManager
./build.sh                                   # 一键编译 + 运行
# 或手动：
javac -d out -encoding UTF-8 $(find src -name "*.java")
java -cp out com.campus.cli.Main
```

首次启动自动生成种子数据：学生 `S001/S002`（密码 `p`）、教师 `T001`、管理员 `A001`。

## 核心知识点

- **8 种集合按场景选用**：HashMap/TreeMap/LinkedHashMap/HashSet/TreeSet/PriorityQueue/ArrayList/LinkedList 各司其职
- **多角色多态**：`abstract User` + `showMenu()` 多态分发，加角色无痛
- **双索引一致性**：主键索引 + 时间索引，写操作走 `addReservationConsistently` 原子方法
- **`TreeMap.subMap` 区间查询**：O(log n + k) 时段冲突检测
- **`LinkedHashMap` LRU**：3 参数 + `removeEldestEntry` 21 行实现最近浏览缓存
- **`PriorityQueue` Top-K**：小顶堆维护前 K 大（O(n log k) 优于全排序）
- **Stream + Lambda**：`filter`/`groupingBy`/`partitioningBy`/`averagingDouble`/方法引用
- **RFC 4180 CSV 转义**：解决"字段含逗号"的真实工程难题

## 关键陷阱（新手经典坑）

1. **ConcurrentModificationException**：遍历 `values()` 时 `remove` → 用 `removeIf` / `Iterator.remove`
2. **双索引不一致**：只更主索引、忘更时间索引 → 冲突检测失效 → 统一走原子方法
3. **CSV 字段错位**：朴素 `split(",")` 遇含逗号字段一刀两半 → 用 `CsvUtil.split`
