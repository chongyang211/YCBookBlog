# JsonManager —— C语言 Json与内存数据库

《C语言入门精通》综合案例 04 的配套源码：**零第三方依赖**手写 mini cJSON + 序列化框架雏形 + MiniDB 内存数据库（REPL）。

教程地址：`packages/website/11.CodeX/01.C语言入门精通/02.综合案例/04.Json与内存数据库.md`

## 编译运行

```bash
make run        # 编译 + 启动 REPL（自动建 data/ 目录）
```

## 一轮完整会话

```text
> SHOW TABLES
 - student (4 行)
 - course (0 行)
> SELECT student WHERE age > 20
  {"student_id":"S002","name":"李四","age":21}
  {"student_id":"S004","name":"赵六","age":22}
> INSERT student S005 钱七 23
✅ 已插入
> DELETE student S003
✅ 已删除
> QUIT          # 自动保存到 data/*.json，重启数据回来
```

## 模块结构（与教程章节对应）

| 文件 | 教程章节 | 职责 |
|---|---|---|
| `src/json_error.h/.c` | §04 | 错误码 + 行列号上下文（C 的"异常体系"）|
| `src/json_value.h/.c` | §03 | tagged union AST + 递归释放 + 平行数组保序 |
| `src/strbuf.h/.c` | §06 | 动态字符串（翻倍扩容 / vsnprintf 两遍法）|
| `src/json_parser.h/.c` | §05 | 递归下降解析器 + `\uXXXX`→UTF-8 + 代理对 + goto fail |
| `src/json_writer.h/.c` | §06 | compact / pretty 双模式 + 转义（`json_escape_write` 导出共用）|
| `src/json_schema.h` | §07 | `field_desc_t`/`struct_desc_t` + FIELD 宏（手写反射灵魂）|
| `src/json_mapper.h/.c` | §08 | 按表双向映射 + new/free/clone 三件套 |
| `src/repository.h/.c` | §09 | void* + 描述表的"泛型仓储" + 深拷贝 |
| `src/minidb.h/.c` | §10 | 多表注册 + fail-fast 加载/保存 |
| `src/entity.h/.c` | §07 | X-Macro 字段清单：一份声明生成结构体 + 描述表 |
| `src/main.c` | §11 | REPL + SELECT WHERE 查询引擎 |

## 已实现的教程挑战（§14.3）

| 挑战 | 状态 | 实现位置 |
|---|---|---|
| A · JSON5（注释 / 单引号 / 尾逗号 / 裸 key）| ✅ | `json_parser.c` 的 `skip_ws`/`parse_key`/`parse_string` |
| B② · `struct_to_json` 复用标准转义 | ✅ | `json_escape_write` 导出，`json_mapper.c` 字符串字段复用 |
| B① · JSON Path（`$.users[0].name`）| 未实现 | 留给读者练习 |
| C · X-Macro 一份声明生成两样 | ✅ | `entity.h` 的 `STUDENT_FIELDS_X` / `COURSE_FIELDS_X` |

另补全：`\uD83D\uDE00` 代理对 → 4 字节 UTF-8（emoji）编码链路。

`data/student.json` 是 JSON5 风格示例（注释/裸 key/尾逗号），启动时被宽松加载，QUIT 后重写为标准 JSON——演示"读宽松、写标准"。

## 剩余局限（教程 §12.3）

- 查询全表扫描 O(n)（索引留给 06 KV 案例）
- 单线程（锁留给 05 案例）
