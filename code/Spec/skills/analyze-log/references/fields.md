# ES 字段字典（test-gz / hk 通用）

基于 `GetEsFields(dataflowName_list=[palmactivationgateway_log_test_gz])` 实测；HK 环境接入名（`*_log_hk`）与 test-gz 共用同一套 ES mapping，字段名与类型一致。大多数接入名字段一致，个别业务专属字段需临时 `GetEsFields` 确认。

> APM 索引（`apm_15692_test_gz_677a_*` / `apm_15692_hk_b2bd_*`）字段结构特殊，需要用 `GetEsFields` 单独查，不适用本文档字段字典。

## 分类速查

### 时间 & 元信息

| 字段 | 类型 | 说明 |
|---|---|---|
| `@timestamp` | date | 日志时间戳（毫秒），排序/时间桶必用 |
| `@offset` | number | 采集偏移量（一般不用） |
| `@source` | keyword | 采集源文件路径 |
| `@host` | keyword | 采集主机 IP |
| `@message` | text | 原始日志一整行（做全文搜索用 `match_phrase`） |
| `log_type` | keyword | 日志类型标记 |

### K8s / 部署（`@`前缀是采集侧注入）

| 字段 | 类型 | 说明 |
|---|---|---|
| `@namespace` | keyword | k8s 命名空间 |
| `@workloadname` | keyword | Deployment/StatefulSet 名 |
| `@workloadtype` | keyword | 工作负载类型 |
| `@podname` | keyword | pod 全名（含 hash 后缀，可用来定位单个副本） |
| `@container` | keyword | 容器名 |
| `@nodename` | keyword | k8s 节点名（一般是 IP） |
| `@nodeip` | keyword | 节点 IP |
| `server_name` | keyword | 应用自报的服务名 |

### 日志内容（业务应用自己 log 出来的）

| 字段 | 类型 | 说明 |
|---|---|---|
| `level` | keyword | `error` / `warn` / `info` / `debug`，最高频过滤字段 |
| `msg` | keyword | 结构化日志的短消息字段（**可 terms 聚合**） |
| `file` | keyword | 源文件相对路径 |
| `func` | keyword | 函数名 |
| `line` | number | 源码行号 |
| `error` | keyword | 错误对象（stringified） |
| `event` | keyword | 事件名（业务自定义） |

### 分布式追踪

| 字段 | 类型 | 说明 |
|---|---|---|
| `trace_id` | keyword | 全链路 trace ID（32 位 hex），跨服务串联 |
| `span_id` | keyword | 当前 span ID（16 位 hex） |
| `session_id` | keyword | 会话 ID（部分接口有） |
| `infra` | keyword | 基础设施标记 |

### HTTP / 网关（gateway 类接入名重点用）

| 字段 | 类型 | 说明 |
|---|---|---|
| `method` | keyword | HTTP 方法（GET/POST/...） |
| `url` | keyword | 完整 URL 或 path |
| `query` | keyword | Query string |
| `status_code` | number | HTTP 响应码 |
| `cost` | number | 请求耗时（**毫秒**） |
| `duration` | keyword | 耗时（字符串形式，如 "1.234s"，兼容旧日志） |
| `request` | keyword | 请求体（可能被截断） |
| `response` | keyword | 响应体（可能被截断） |
| `request_size` | number | 请求字节数 |
| `response_size` | number | 响应字节数 |
| `request_compressed` | boolean | 请求是否压缩 |
| `response_compressed` | boolean | 响应是否压缩 |
| `content_type` | keyword | Content-Type |
| `remote_addr` | keyword | 客户端 IP |

### DB / SQL

| 字段 | 类型 | 说明 |
|---|---|---|
| `sql` | keyword | 执行的 SQL |
| `rows` | number | 影响/返回的行数 |
| `duration` | keyword | SQL 耗时（字符串） |

（cost 是通用耗时，DB 场景下也有效）

### 业务字段（palm/wecard 常见）

| 字段 | 类型 | 说明 |
|---|---|---|
| `biz_code` | keyword | 业务错误码 |
| `device_sn` | keyword | 设备序列号（palm-* / device-* 服务重点字段） |

## 使用注意

### keyword vs text

- **keyword**（大多数字段）：可 `term` 精确匹配、可 `terms` 聚合、可 `sort`
- **text**（`@message`）：只能 `match` / `match_phrase`，**不能 aggs、不能 sort**
- 一个字段的两种存储可能同时存在（如 `msg` 在有的接入名里是 keyword，在有的里是 text），拿不准就先 `GetEsFields`

### 数值字段陷阱

- `cost` 是**毫秒**（单位很重要，`>3000` 才是 3 秒）
- `duration` 是**字符串**（历史遗留，用 `cost` 替代之）
- `status_code` 是 long，用 `range` 而不是 `term` 匹配多个值

### 常用组合

- 找一个 trace 的入口：`term(trace_id)` + `term(server_name)="apigateway"` + `sort(@timestamp asc, size:1)`
- 找一个 pod 的问题：`term(@podname)` + `term(level:error)`
- 找一类接口的慢请求：`term(url)` + `range(cost:>3000)` + `sort(cost desc)`
- 找一个设备的所有日志：`term(device_sn)` 跨接入名合并查
