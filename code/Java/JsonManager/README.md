# JsonManager — Java JSON 与内存数据库

《Java 入门精通》综合案例的**第四关·现代 Java 大考**。手写一个**零第三方依赖**的 mini Jackson + mini MyBatis 雏形，用纯 JDK 17 的**反射 + 注解 + 泛型 + sealed/record**四件套还原工业级 JSON 框架的核心机制。

## 功能（12 项）

**JSON 解析/序列化**：
- 递归下降解析任意嵌套 JSON → `JsonValue` AST
- compact / pretty 双模式序列化
- round-trip 完整保真（含 `\uXXXX` Unicode 转义）

**对象映射**：
- POJO ↔ JSON 双向映射（`ObjectMapper`）
- `@JsonField(name=...)` 字段名映射、`required=true` 必填校验
- `@JsonIgnore` 跳过字段
- 嵌套对象 + `List<T>`/`Map<K,V>` 泛型集合自动还原

**内存数据库**：
- `Repository<T>` 泛型仓储 + 通配符 PECS
- `MiniDB` 多表动态注册 + 启动加载/关闭保存
- REPL：`SELECT student WHERE age > 20`

## 项目结构（7 包 / 22 类）

```text
src/com/minijson/
├── json/       JsonValue(sealed) + 6 record + JsonParser(递归下降) + JsonWriter
├── exception/  JsonException + Parse(行列号)/Type/KeyMissing
├── annotation/ @JsonField + @JsonIgnore
├── mapper/     ObjectMapper(反射映射，mini Jackson)
├── db/         Repository<T>(泛型仓储) + MiniDB
├── entity/     Student + Course(含 List<Student> 泛型集合)
└── cli/        QueryEngine(SELECT WHERE) + Main(REPL)
```

## 编译运行

```bash
cd JsonManager
./build.sh        # 一键编译 + 运行（需 JDK 17+）
# 或手动：
javac -d out -encoding UTF-8 --release 17 $(find src -name "*.java")
java -cp out com.minijson.cli.Main
```

## 核心知识点

- **sealed interface + record**：6 种 JSON 类型用封闭代数表达，`switch` 模式匹配编译器强制穷尽
- **递归下降解析器**：文法规则 ↔ Java 方法互相调用，行列号错误定位，`\uXXXX` 转义
- **注解驱动映射**：`@JsonField` + `@Retention(RUNTIME)`，`Field.getAnnotation` 反射读取
- **反射突破类型擦除**：`Field.getGenericType()` + `ParameterizedType.getActualTypeArguments()` 还原 `List<Student>`
- **泛型仓储**：构造接 `Class<T>` 绕过擦除，通配符 PECS（`? extends` 读 / `? super` 写）
- **IdentityHashMap**：循环引用检测（`==` 判等，避免 equals 递归）
- **REPL + 反射查询**：`SELECT <table> WHERE <field> <op> <value>` 用 `Field.get` 比较

## 关键陷阱（框架级经典坑）

1. **Unicode 转义**：不处理 `\uXXXX` → 中文乱码 → `Integer.parseInt(hex, 16)`
2. **类型擦除**：用 `Class<?>` 而非 `ParameterizedType` → `List<Student>` 变 `List<HashMap>`，下游 ClassCastException
3. **必填字段缺失**：JSON 缺 `@JsonField(required=true)` 字段 → 静默 null → 下游 NPE 找不到源头 → `JsonKeyMissingException` 精准报错

## 示例数据格式

`data/student.json`（`@JsonField` 字段名映射 + `@JsonIgnore` 跳过 password）：
```json
[
  {
    "student_id": "S001",
    "name": "张三",
    "age": 20
  }
]
```
