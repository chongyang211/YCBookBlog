# Zerus API 契约

创建 zerus 联调环境涉及的后台接口清单。每次新增一个接口，就往下追加一节；
`run_automation.py` 里的常量 / 请求逻辑以本文档为唯一事实来源。

- Base URL: `https://zerus.woa.com`
- 鉴权：`Cookie`（由 `login_and_save.py` 产出的 `cookies.json`）
- 默认 `Content-Type: application/json`
- **传输**：zerus 网关只接受 HTTP/2。Python `requests`（HTTP/1.1）会在 TLS 握手阶段被断连
  (`SSLError: record layer failure`)。工程侧目前的做法是**用 curl `--http2` 发请求，Python 只负责拼参数 / 解析 JSON**（见 `_probe_via_curl.py`）。后续 `run_automation.py`
  接入正式接口时，也建议改走 `httpx[http2]` 或 `subprocess.run(["curl", ...])`。

> ⚠️ 敏感说明：本文所有示例中的 `Cookie` 值均为占位（`<RIO_TOKEN>` 等），
> **不要**把真实 cookie 写进本文件或提交到仓库。

## 抓取真实响应

```bash
cd skills/create-zerus/zerus_cookie
python login_and_save.py                    # 拿新 cookie（cookie 过期时执行）
python _probe_via_curl.py                   # 跑 §1 + §2 前 2 个服务，落到 _probe_out/
python _probe_via_curl.py --service-id 711  # 只探单个服务
```

`_probe_out/` 里的 JSON 是回填本文档字段表的一手资料，**不要提交**（属于内部数据）。

---

## 1. 查询项目已接入的服务列表

用途：拉取 zerus 当前项目下所有已接入的服务，其 `name` 与代码仓库 `src/<service>/` 一一对应，
作为「创建环境」时选择 / 覆盖服务的候选集。

### 请求

- Method: **`POST`**（注意：路径以 `_query` 结尾看似 GET，实际是 POST + JSON body。原始抓包 curl 用 `--data`，服务端只注册了 POST；错发 GET 会返回 `code=12, "failed to match any pattern"`）
- Path: `/api/v4/projects/{project_id}/services_query`
- 当前使用的 `project_id`: `71`
- Query 参数：
  | 参数 | 类型 | 必选 | 默认 | 说明 |
  |------|------|------|------|------|
  | `page` | int | 是 | `1` | 页码 |
  | `pageSize` | int | 是 | `99999` | 单页数量。当前用超大值一次性拉全量，避免分页 |
- Body（JSON）：
  ```json
  { "name": "" }
  ```
  `name` 为服务名的模糊过滤条件，传空串代表不过滤、返回全部。

### curl 示例

```bash
curl -X POST 'https://zerus.woa.com/api/v4/projects/71/services_query?page=1&pageSize=99999' \
  --http2 \
  -H 'Cookie: P_RIO_TOKEN=<P_RIO_TOKEN>; x-client-ssid=<SSID>; x_host_key_access_https=<HOST_KEY>' \
  -H 'Content-Type: application/json' \
  --data '{ "name": "" }'
```

### 响应（已用真实数据校对，见 `_probe_out/services_query.json`）

- HTTP `2xx`，Body 为 JSON。
- 顶层结构：
  ```jsonc
  {
    "code": 0,           // 业务码，0 表示成功
    "message": "OK",
    "data": {
      "page": 1,
      "pageSize": 99999,
      "count": 49,        // ⚠️ 观测到 count(49) > list.length(33)，可能包含软删除 / 无权限项，
                           //    以 list 实际长度为准
      "list": [ /* Service[] */ ]
    }
  }
  ```
- `data.list[]` 元素（`Service`）—— 字段以实际响应为准：
  | 字段 | 类型 | 含义 | 用途 |
  |------|------|------|------|
  | `id` | int | 服务 ID | 传给 §2 / 创建环境接口 |
  | `name` | string | 服务名 | **与 `src/<name>/` 仓库目录一致**，是本 skill 匹配变更仓库 → zerus 服务的主键 |
  | `desc` | string | 描述 | 展示用，可空 |
  | `projectID` | int | 所属项目 ID | 校验用，应等于路径里的 `project_id` |
  | `enable` | bool | 是否启用 | `false` 的服务应过滤掉 |
  | `creator` | string | 创建人 rtx | 展示用 |
  | `createdAt` / `updatedAt` | string(RFC3339) | 时间戳 | 展示用 |
  | `serviceMetas` | array | 发布通道列表 | **§1 响应里此字段是空数组**，需在 §2 详情接口才有完整内容 |
  | `requireServices` | array | 依赖服务 | 目前均为空数组，语义待确认 |

  ⚠️ 本响应**不含** `gitlab 地址 / 默认分支 / owner` 等字段，如后续需要要另找接口。

### 错误 & 边界

| 情形 | 处理 |
|------|------|
| `401` / `302 → 登录页` / HTML 含"登录" | Cookie 过期，回 SKILL.md Step 1 重新登录，重试**一次** |
| `2xx` 但 `code != 0` | 展示 `message`，不重试，交回用户 |
| `5xx` / 超时 | 指数退避重试 ≤ 2 次（2s / 5s） |
| `data.list` 为空 | 报错停止 —— 项目下应至少有 1 个服务，多半是 `project_id` 传错 |

### 在 skill 里的使用位置

- Step 2「汇总本次改动涉及的服务与分支」：
  1. 调本接口拿到 `services` 全量列表；
  2. 用 `src/` 下改动仓库的目录名与 `service.name` 对齐，产出 `changes[]` 草稿；
  3. 未在 `services` 中出现的本地目录 → 提示用户人工确认（zerus 未接入或名称不一致）。

---

## 2. 查询单个服务的详情（含构建 / 发布通道元数据）

用途：在 §1 拿到候选服务列表后，对**每个需要覆盖分支的服务**再拉一次详情，
获取该服务的**发布通道（serviceMetas）**——里面记录了构建系统（蓝盾流水线 ID）
和发布系统（k8s deployment 名 / namespace）配置，是「创建环境」请求体的核心组成部分。

> ⚠️ **本接口本身不返回流水线的可变参数**（分支、镜像 tag 等）。
> 它只给出 `buildSystemMeta.bk.bkPipelineID` —— 真正的**流水线参数模板**要再调 §3
> `GET .../meta/{serviceMetaID}/bk_params`（仍走 zerus 网关，不必直连蓝盾）。

### 请求

- Method: `GET`
- Path: `/api/v4/projects/{project_id}/services/{service_id}`
  - `project_id`：与 §1 相同，当前为 `71`
  - `service_id`：来自 §1 响应 `data.list[].id`
- Query / Body：无
- 请求头：仅需 `Cookie`

### curl 示例

```bash
curl 'https://zerus.woa.com/api/v4/projects/71/services/713' \
  --http2 \
  -H 'Cookie: P_RIO_TOKEN=<P_RIO_TOKEN>; x-client-ssid=<SSID>; x_host_key_access_https=<HOST_KEY>'
```

### 响应（已用真实数据校对，见 `_probe_out/service_713.json`）

顶层：
```jsonc
{
  "code": 0,
  "message": "OK",
  "data": { /* Service 详情 */ }
}
```

`data` 字段（在 §1 的 Service 基础上，`serviceMetas` 被填充）：

| 字段 | 类型 | 说明 |
|------|------|------|
| `id` / `name` / `desc` / `projectID` / `enable` / `creator` / `createdAt` / `updatedAt` | — | 同 §1 |
| `serviceMetas` | `ServiceMeta[]` | **发布通道**列表，每个服务通常至少 1 条，见下 |
| `requireServices` | array | 依赖的其他服务，目前均为空 |

**`ServiceMeta` 结构**（真实样例）：

| 字段 | 类型 | 说明 | 样例值 |
|------|------|------|--------|
| `id` | int | 通道 ID | `1396` |
| `projectID` / `serviceID` | int | 冗余外键 | — |
| `type` | string | 通道类型 | `"vt"`（观察到的唯一取值，语义待确认） |
| `name` | string | 展示名 | `"蓝盾流水线构建镜像"` / `"基线镜像"` |
| `desc` | string | 描述 | 通常为空 |
| `headerDomain` | string | 域名前缀？ | 通常为空 |
| `extend` | string(JSON) | 扩展字段，序列化后的 JSON | `"{\"customHeaders\":[]}"` |
| `buildSystemMeta` | object | **构建系统配置**，见下 | — |
| `releaseSystemMeta` | object | **发布系统配置**，见下 | — |

**`buildSystemMeta`**（构建走哪套系统）：

| 字段 | 类型 | 说明 | 样例 |
|------|------|------|------|
| `id` | int | 与所在 `serviceMeta.id` 相同 | `1396` |
| `buildSystemID` | int | 构建系统 ID（zerus 内部编号） | `74` |
| `serviceID` / `serviceMetaID` | int | 冗余外键 | — |
| `type` | string | 构建系统类型 | `"bk"`（蓝盾） |
| `base` | any | 基础镜像相关？ | `null`（观察值） |
| `bk` | object | type=`bk` 时的具体配置 | `{ "bkPipelineID": "p-bd32a4f7504e4753aa1cca0faae0b843" }` |
| `bk.bkPipelineID` | string | **蓝盾流水线 ID**，`p-<hex32>` 格式 | 用于拉取流水线参数 & 触发构建 |

**`releaseSystemMeta`**（发布走哪套系统）：

| 字段 | 类型 | 说明 | 样例 |
|------|------|------|------|
| `id` | int | 同 serviceMeta.id | `1396` |
| `releaseSystemID` | int | 发布系统 ID | `45` |
| `serviceID` / `serviceMetaID` | int | 冗余外键 | — |
| `type` | string | 发布类型 | `"direct"`（k8s 直发；其他取值待观察） |
| `direct` | object | type=`direct` 时的部署配置 | 见下 |
| `direct.namespace` | string | k8s 命名空间 | `"default"` |
| `direct.cloneDeploymentName` | string | 从哪个 deployment 克隆 | `"palmdevicemanage"` |
| `direct.cloneServiceName` | string | 从哪个 service 克隆 | `""`（可空） |
| `direct.selectorServiceName` | string | 环境选路要用的 service 名 | `"palmdevicemanage"` |
| `direct.containerName` | string | 目标容器名 | `"palmdevicemanage"` |
| `direct.timeout` | int | 发布超时（秒） | `600` |

### 错误 & 边界

| 情形 | 处理 |
|------|------|
| `401` / `302 → 登录页` | Cookie 过期，回 SKILL.md Step 1 重新登录，重试**一次** |
| `404` | `service_id` 不属于该 `project_id`，或服务已下线；停止，交回用户确认 |
| `2xx` 但 `code != 0` | 展示 `message`，不重试 |
| `5xx` / 超时 | 指数退避重试 ≤ 2 次（2s / 5s） |
| `data.serviceMetas` 为空 | 该服务未配置发布通道，无法构建，交回用户确认 |
| `data.serviceMetas.length > 1` | 存在多个发布通道，需要让用户选（或按 `name` 匹配），先记录 TBD |

### 在 skill 里的使用位置

- Step 2 拿到 `changes[]` 草稿后，对**每个变更服务**依次调用本接口：
  - 提取 `serviceMetas[i].buildSystemMeta.bk.bkPipelineID` → 存入 change 上下文
  - 提取 `serviceMetas[i].releaseSystemMeta.direct.*` → 校验 k8s 目标 & 拼装发布参数
- Step 3 用户确认时展示：`service.name` / feature 分支 / 命中的发布通道 `serviceMeta.name` / `bkPipelineID`（短哈希即可）
- Step 4 组装「创建环境」请求体时，`changes[]` 每项需要带：
  - `serviceID` / `serviceMetaID`
  - feature 分支（来自 git 侧）
  - 从 §3 拉到的流水线参数值（覆盖 `GIT_BRANCH_PROJECT` 等）
  - 发布覆盖项（如果需要改 namespace / image tag）

---

## 3. 查询发布通道对应的蓝盾流水线参数模板

用途：拿到 §2 里某个 `serviceMeta` 的**流水线可传入参数列表**（每项含默认值、类型、是否必填、绑定的代码库等）。**这是本 skill 的核心数据**：feature 分支就是要覆盖到里面的 `GIT_BRANCH_PROJECT`；`ZERUS` 参数决定是否走多环境。

### 请求

- Method: `GET`
- Path: `/api/v4/projects/{project_id}/services/{service_id}/meta/{service_meta_id}/bk_params`
  - `project_id`：与 §1 / §2 相同，当前为 `71`
  - `service_id`：来自 §1 `data.list[].id`
  - `service_meta_id`：来自 §2 `data.serviceMetas[].id`
- Query / Body：无
- 请求头：仅需 `Cookie`

> 虽然名字带 `bk_`（蓝盾），但**仍走 zerus 网关**，用同一份 cookie 即可，不用直连 `devops.woa.com`。

### curl 示例

```bash
curl 'https://zerus.woa.com/api/v4/projects/71/services/711/meta/1394/bk_params' \
  --http2 \
  -H 'Cookie: P_RIO_TOKEN=<P_RIO_TOKEN>; RIO_TOKEN=<RIO_TOKEN>; x-client-ssid=<SSID>; x_host_key_access_https=<HOST_KEY>'
```

### 响应（已用真实数据校对，见 `_probe_out/bk_params_s711_m1394.json` / `bk_params_s713_m1396.json`）

顶层：
```jsonc
{
  "code": 0,
  "message": "OK",
  "data": {
    "version": 2,          // 流水线参数模板版本号，蓝盾每编辑一次 +1
    "params": [ /* BkParam[] */ ]
  }
}
```

**`BkParam` 结构**（所有字段都出现在每一项里，缺省用空串 / `false`）：

| 字段 | 类型 | 说明 |
|------|------|------|
| `id` | string | 参数名，蓝盾里的变量 key，如 `GIT_BRANCH_PROJECT` |
| `required` | bool | 是否必填。触发构建时必填项必须传值（或至少让默认值非空） |
| `type` | string | 参数类型，观察到的取值：`STRING` / `SUB_PIPELINE`（关联流水线）/ `CODE_LIB`（代码库选择器） |
| `defaultValue` | string | 默认值。skill 里除了 `GIT_BRANCH_PROJECT` 与 `ZERUS`，其余保持默认即可 |
| `desc` | string | 参数中文描述，展示用 |
| `elementAtomCode` | string | 蓝盾插件类型标记，`type=STRING` 一般为空，代码库分支参数会是 `"gitCodeRepo"` |
| `repositoryHashID` | string | 蓝盾内部代码库 ID（type=STRING+`elementAtomCode=gitCodeRepo` 时有值） |
| `aliasName` | string | 代码库路径，如 `palm/weixin/device_manage`。**可用来交叉验证本地 `src/<name>/` → zerus service** |
| `url` | string | 代码库 clone URL，如 `https://git.woa.com/palm/weixin/device_manage.git` |
| `auto` | bool | 未知，观察到均为 `false` |
| `listenPath` | string | 未知，观察到均为空 |

### 观察到的固定参数集合（12 项）

按出现顺序（两个不同服务一致，仅默认值不同）：

| # | `id` | `required` | `type` | 说明 / skill 里的处理 |
|---|------|------------|--------|------------------|
| 0 | `BK_CI_BUILD_MSG` | ✅ | STRING | 构建信息描述。skill 可自动填 `"[cospec] create env for <branch>"` |
| 1 | `RAINBOW_APPID` | | STRING | 七彩石 AppID，默认值即业务配置，不动 |
| 2 | `TKE_CLUSTER_NAME` | | STRING | TKE 集群凭证，默认 `kubeconfig_qcloud_palm_gz_test`，不动 |
| 3 | `IMAGE_TICKET` | | STRING | 镜像仓库凭证，默认 `bk_docker_repo`，不动 |
| 4 | `RAINBOW_ENV` | | STRING | 七彩石配置环境，默认 `test-gz`，不动 |
| 5 | `TKE_NAMESPACE` | | STRING | TKE 命名空间，默认 `default`，不动 |
| 6 | `TARGET_IMAGE_REPO` | | STRING | 目标镜像仓库地址，不动 |
| 7 | `TARGET_IMAGE_TICKET` | | STRING | 目标镜像仓库凭证，不动 |
| 8 | **`GIT_BRANCH_PROJECT`** | ✅ | STRING (`elementAtomCode=gitCodeRepo`) | **构建分支** —— 默认 `develop`；本 skill **必须**覆盖为用户选定的 feature 分支。同项自带 `url` / `aliasName`，用于确认目标仓库 |
| 9 | `INIT_PIPELINE` | | SUB_PIPELINE | 关联构建流水线（`[CONFIG] xxx`），不动 |
| 10 | **`ZERUS`** | | STRING | **泽鲁斯多环境管理标识**，默认空 = 走默认发布；**skill 需填非空值**（推测填环境 ID / name，具体等 §4 创建环境接口敲定后回填） |
| 11 | `GIT_REPO_ALIAS` | | CODE_LIB | 代码库别名，等于 `GIT_BRANCH_PROJECT.aliasName`，不动 |

⚠️ 上表基于两条真实样本（`s711/m1394` / `s713/m1396`）归纳，字段集合完全一致。若后续遇到其他 service 的 meta 参数集不同，需在本表补充。

### 错误 & 边界

| 情形 | 处理 |
|------|------|
| `401` / `302 → 登录页` | Cookie 过期，回 SKILL.md Step 1 重新登录，重试**一次** |
| `404` | `service_meta_id` 不属于 `service_id`，或该通道被删除；停止，交回用户确认 |
| `2xx` 但 `code != 0` | 展示 `message`，不重试 |
| `5xx` / 超时 | 指数退避重试 ≤ 2 次（2s / 5s） |
| `data.params` 为空 | 流水线未配置任何参数；异常状态，交回用户确认 |
| `GIT_BRANCH_PROJECT` 缺失 | 该流水线不由代码分支驱动，本 skill 场景不支持，交回用户确认 |
| `GIT_BRANCH_PROJECT.url` 与仓库预期不匹配 | 说明 zerus service ↔ 本地目录映射错了，中止并要求用户核对 |

### 在 skill 里的使用位置

- Step 2 对每个变更服务调完 §2 后，对每个 `serviceMeta.id` 再调本接口，得到 `params[]` 模板
- 用**变更集里对应仓库的 feature 分支**覆盖 `GIT_BRANCH_PROJECT.defaultValue`
- 用 `GIT_BRANCH_PROJECT.aliasName` / `url` **反向校验**：这个 zerus service 确实指向本地 `src/<name>/` 那个仓库；不匹配则停止
- 组装 §4 「创建环境」请求体时，把整份 `params[]`（含被覆盖的分支）**原样**回传（`ZERUS` 目前保持默认空即可 —— 抓包样本也是空）

---

## 4. 创建环境（Create Env）

用途：把「一套 feature 分支 → 一套联调环境」这件事真正落地。**本 skill 的终点接口**。

### 请求

- Method: `POST`
- Path: `/api/v4/projects/{project_id}/envs`
  - `project_id` = §1 / §2 / §3 里用的项目 ID，当前 `71`
- Content-Type: `application/json`
- 请求头：仅需 `Cookie`（同前几节，`.woa.com` 父域 cookie 已够）

### 请求体（基于真实抓包 `_probe_out/create_env_sample_request.json`）

顶层：
```jsonc
{
  "id": 0,                    // 服务端分配，传 0
  "name": "test",             // 环境名，**skill 需覆盖**：与当前会话 / spec 相关，见下方"字段填法"
  "hashName": "",             // 服务端分配，传空
  "type": "vt",               // 环境类型：vt = virtual test（虚拟联调）。目前只见过这一个值
  "desc": "",                 // 备注，可选。skill 可填 spec 标题
  "projectID": 0,             // 服务端从 URL 上取，body 里传 0 即可
  "endTime": "2026-07-20T07:17:00.152Z",  // **过期时间**，ISO 8601 带 Z (UTC)。skill 默认 = 当前时间 + 7d
  "creator": "",              // 服务端从 cookie 取，传空
  "prevent": false,           // 未知，抓包为 false，保持
  "status": 0,                // 服务端分配，传 0
  "serviceInstanceList": [ /* ServiceInstance[] —— 每个变更服务一项 */ ],
  "envCustom": {
    "additionEnvVars": []     // 附加环境变量，skill 场景暂不用，传空数组
  }
}
```

**`ServiceInstance` 结构**（每个变更服务一项）：

```jsonc
{
  "id": 0,                    // 传 0
  "envID": 0,                 // 传 0
  "serviceID": 711,           // ← §1 拿到的 service.id
  "serviceMetaID": 1394,      // ← §2 拿到的 serviceMetas[*].id（选中的发布通道）
  "name": "palmdiagnosis",    // ← §1/§2 的 service.name，做人类可读标记
  "status": "",
  "type": "vt",               // 与顶层 type 一致
  "buildInfo": {
    "type": "bk",             // 构建类型：bk = 蓝盾
    "base": {
      "autoNew": false        // 布尔含义："是否创建时立刻跑一次首轮构建"。**与 push 触发无关**。skill 默认 **false**（不立即首建）
    },
    "bk": {
      "projectID": "",        // 传空，服务端补
      "pipelineID": "",       // 传空，服务端补（§2 里已知的 bkPipelineID 也不用手填）
      "pipelineVersion": 0,
      "pipelineStatus": "",
      "buildID": "",
      "params": [ /* ← §3 bk_params.data.params 整份原样回传，仅覆盖 GIT_BRANCH_PROJECT.defaultValue */ ],
      "triggerMode": "",
      "triggerUser": ""
    }
  },
  "releaseInfo": {
    "type": "direct",         // 发布类型：direct = 直接发布（不再走蓝盾发布流水线）
    "base": {
      "autoNew": false        // 语义同 buildInfo.base："是否创建时立刻发布一次"。skill 默认 **false**
    },
    "bk": {                   // type=direct 时整块保留、全空
      "projectID": "",
      "pipelineID": "",
      "pipelineVersion": 0,
      "pipelineStatus": "",
      "buildID": "",
      "params": [],
      "triggerMode": "",
      "triggerUser": ""
    },
    "direct": {               // 全空即可，服务端自己决定目标集群/命名空间
      "clusterArea": "",
      "namespace": "",
      "cloneDeploymentName": "",
      "cloneServiceName": "",
      "selectorServiceName": "",
      "containerName": "",
      "msg": ""
    }
  },
  "creator": ""
}
```

### 字段填法（skill 视角，只列**需要覆盖 / 需要动脑**的字段）

| 位置 | skill 填法 |
|------|-----------|
| `name` | **必填**。与当前会话相关：默认 `spec-<STORYID>-<slug>`（story id 从 spec 文件名取），用户可覆盖。抓包里为演示用的 `"test"`，skill 不能沿用 |
| `endTime` | **必填**。默认 `now() + 7 days`，格式 `YYYY-MM-DDTHH:mm:ss.sssZ`（UTC）。用户可指定其他有效期 |
| `type` (顶层 & instance) | 固定 `"vt"` |
| `desc` | 可选，建议填 spec 标题或"spec 0003 联调环境"这种一句话说明 |
| `serviceInstanceList[]` | 每个"变更服务"一项。**未变更服务不进这个列表**，走默认基线 |
| `serviceID` / `serviceMetaID` | 来自 §1 + §2，别搞混：`serviceMetaID` 必须属于该 `serviceID` |
| `name` (instance) | 用 §1 里的 `service.name`（zerus 上的服务名，非仓库名）|
| `buildInfo.base` | 传 `{"autoNew": false}`（默认，不立即首建）。**与 push 触发无关** —— 见下方 §"Git push 自动触发的真正开关" |
| `buildInfo.bk.params` | 用 §3 拿到的整份 `params` **原样带回**；把该服务对应仓库的 feature 分支填到 `GIT_BRANCH_PROJECT.defaultValue`，**并把 `GIT_BRANCH_PROJECT.auto` 强制置为 `true`**（这是 push 触发的真正开关）。可选顺手填 `BK_CI_BUILD_MSG.defaultValue = "[cospec] create env for <branch>"` |
| `releaseInfo.type` | 固定 `"direct"`（抓包样本）|
| `releaseInfo.base` | 传 `{"autoNew": false}`（默认，不立即发布），语义与 `buildInfo.base` 一致 |
| `releaseInfo.bk` | 保留字段但全空（如上模板）|
| `releaseInfo.direct.*` | 全空 |
| `envCustom.additionEnvVars` | 空数组 |

### 关于 `params` 覆盖的具体规则

1. **深拷贝** §3 拿到的 `data.params[]`，不要就地改
2. 遍历，找到 `id == "GIT_BRANCH_PROJECT"` 的那项：
   - `defaultValue` 改为该服务对应仓库的 feature 分支名
   - **`auto` 必须强制置为 `true`** —— 这是 zerus 环境「Git push 自动触发」的真正开关（用户 UI 抓包对比：`auto=false` 无触发，`auto=true` 才有）
3. **可选**：找到 `id == "BK_CI_BUILD_MSG"` 的那项，把 `defaultValue` 改为一句可追溯的构建说明
4. **不要动** `ZERUS`（保持默认空 —— 抓包样本就是空，服务端会按环境类型 `vt` 自己处理多环境路由）
5. 其他 10 项**一个字段都别动**（含 `elementAtomCode` / `repositoryHashID` / `aliasName` / `url` 等），原样回传

### curl 示例（去 cookie）

```bash
curl 'https://zerus.woa.com/api/v4/projects/71/envs' \
  --http2 \
  -X POST \
  -H 'Content-Type: application/json' \
  -H 'Cookie: P_RIO_TOKEN=<...>; RIO_TOKEN=<...>; x-client-ssid=<...>; x_host_key_access_https=<...>' \
  --data @create_env_body.json
```

完整请求体样例见 `_probe_out/create_env_sample_request.json`。

### 响应（实测 2026-07-13，样本 `_probe_out/create_env_response.json`）

顶层照旧 `{code, message, data}`，成功时 `code == 0` / `message == "OK"`：

```jsonc
{
  "code": 0,
  "message": "OK",
  "data": {
    "id": 6593,                                       // ⭐ 环境 ID（数字，用于后续查询 / 删除）
    "name": "cospec-probe-xzh-1783928207",            // 回显创建时传的 name
    "hashName": "7jawpjbngz5rl",                      // ⭐ URL 短标识（zerus 前端拼环境访问路径就用它）
    "type": "vt",
    "desc": "...",
    "projectID": 71,
    "endTime": "2026-07-13T08:36:47.686Z",            // 服务端回显（毫秒精度保留）
    "creator": "kennyzhxu",                           // ⭐ 从 cookie 自动补的工号
    "prevent": false,
    "status": 0,                                      // 环境层状态（0 = 初始）
    "createdAt": "2026-07-13T07:36:48.676Z",
    "updatedAt": "2026-07-13T07:36:48.676Z",
    "serviceInstanceList": [
      {
        "id": 15276,                                  // instance ID（独立于 env.id）
        "envID": 6593,                                // 反引 env.id
        "serviceID": 711,
        "serviceMetaID": 1394,
        "name": "palmdiagnosis",
        "status": "BUILD_WAIT",                       // ⭐ 服务实例状态，触发即为 BUILD_WAIT
        "type": "vt",
        "buildInfo": {
          "type": "bk",
          "base": null,                               // ⚠️ 请求里传的 {autoNew:true} 服务端**不回显**（见下方说明）
          "bk": { /* 完整 params[] 原样回显，含被覆盖的 GIT_BRANCH_PROJECT / BK_CI_BUILD_MSG */ }
        },
        "releaseInfo": {
          "type": "direct",
          "direct": { /* 全空的直接发布配置 */ }
          // ⚠️ 请求里传的 `releaseInfo.base` 和 `releaseInfo.bk` 服务端**都不回显**
        },
        "creator": "kennyzhxu",
        "createdAt": "2026-07-13T07:36:49Z",          // ⚠️ 秒精度（无毫秒），与 env 层的 ISO 格式不完全一致
        "updatedAt": "2026-07-13T07:36:49Z"
      }
    ],
    "envCustom": { "additionEnvVars": [] }
  }
}
```

### Git push 自动触发的真正开关（✅ 用户 UI 抓包 diff 确认）

**结论：push 触发的开关是 `buildInfo.bk.params[GIT_BRANCH_PROJECT].auto`，不是 `base.autoNew`。**

用户在 zerus 前端做了两次相同表单、只切换"Git 推送自动触发"UI 开关的对照抓包（两份 payload 只有**一个字段**不同）：

| 参数 | UI 关闭时 | UI 开启时 |
|---|---|---|
| `bk.params[GIT_BRANCH_PROJECT].auto` | `false` | **`true`** ✅ |
| 其他所有字段（含 `base: {"autoNew": false}`、其余 11 个 params、`releaseInfo`……）| 完全一致 | 完全一致 |

也就是说：
- `GIT_BRANCH_PROJECT.auto = true` → 该参数由 Git 事件自动灌入 branch/commit，push webhook 可触发流水线
- `GIT_BRANCH_PROJECT.auto = false` → 参数固定为 `defaultValue`，push 事件不会触发（即使 base 里写了 `autoNew`）
- `base.autoNew` 布尔仅控制"创建时是否立刻跑一次首轮构建"，**与 push 触发正交**

⚠️ **早期错误结论已作废**：`base` 字段的存在性 / `autoNew` 布尔值 都不是 push 触发开关。

**响应体回显规则（服务端裁剪现象）**：实测响应里 `buildInfo.base` 会变成 `null`、`releaseInfo.base` / `releaseInfo.bk` 直接消失。这是服务端对回显做的裁剪，**不能**据此判断请求是否被接受。判断创建成功**只看**顶层 `code == 0`；`serviceInstanceList[].status == "BUILD_WAIT"` 表示新 instance 已入队。

**skill 端固定策略**：
- `bk.params[GIT_BRANCH_PROJECT].auto` **永远强制置为 `true`**（由 `override_git_branch` 保证）
- `buildInfo.base` / `releaseInfo.base` 都传 `{"autoNew": false}`（不立即首建）
- 仅当用户明确要求"我现在就要出镜像"时，才把 `autoNew` 改为 `true`

### 关于访问入口 URL

**响应里没有直接的 `accessUrl` 字段**。skill 需要按 `hashName` 自己拼环境详情页：
- 环境详情页（推测）：`https://zerus.woa.com/env/{hashName}` 或 `https://zerus.woa.com/project/71/env/{hashName}`（**具体路径待前端路由抓包**）
- 兜底方案：只输出 `data.id` / `data.hashName` / `data.name`，让用户去 zerus 控制台按名字搜

### 错误 & 边界

| 情形 | 处理 |
|------|------|
| `2xx` 且 `code == 0` | 成功；把 `data.id` / `data.hashName` / 访问 URL 交给用户 |
| `2xx` 但 `code != 0` | 展示服务端 `message`，**不重试**；常见：环境名重复、分支在蓝盾侧解析不到 |
| `400` / `422` | 请求体字段错，把 body diff 打到日志（**注意脱敏 cookie**），交回用户 |
| `401` / `302 → 登录页` | Cookie 过期，回 SKILL.md Step 1 重新登录，重试**一次** |
| `403` | 无权限；停止，让用户在 zerus 前端确认账号是否加入该 project |
| `5xx` / 超时 | 指数退避重试 ≤ 2 次（2s / 5s）。**⚠️ 幂等未知** —— 如果服务端已经在创建，重试可能产生**同名冲突**或**双环境**。重试前先跑一次「列表查询」（§5）确认是否已存在同名环境 |
| `endTime` 已在过去 | 服务端可能拒绝；skill 在组装 body 前先本地校验 `endTime > now + 5min` |

### 幂等性 & 重复创建

- 抓包没有幂等 key。同名 `name` 是否允许由服务端决定，观察到什么补什么。
- skill 建议：**创建前用「环境列表」接口过一遍同名环境**（§5）。存在同名 & 未过期时，先让用户选：复用 / 换名 / 覆盖。

### 在 skill 里的使用位置

- 这是 SKILL.md Step 4 的**最终调用**。
- 前置：Step 2 已经收集齐变更服务 + feature 分支；Step 3 已跟用户确认过 payload（**尤其 `name` / `endTime` / 覆盖服务清单**）。
- 调用后：Step 5 输出环境名 + 关联服务与分支表格给用户（用户要求）。

---

## 5. 环境列表 / 查询（List / Query Envs）

用途：
- **创建前防重名**：Step 4 POST 之前，先查一遍同名 & 未过期的环境
- **5xx 重试防双创**：Step 4 5xx 后重试前，再查一次，确认服务端是否已经悄悄创建成功
- **skill 输出\"访问入口\"兜底**：如果 §4 响应没直接给访问 URL，可用这个接口按 `name` / `hashName` 反查环境详情

### 请求

- Method: **`POST`**（注意：**尽管是查询，实际是 POST + JSON body**，不是 GET）
- Path: `/api/v4/projects/{project_id}/envs_query`
  - `project_id` 走 URL，当前 `71`
- Query string（分页在 query 里）：
  - `page` (int, ≥1)
  - `pageSize` (int, 抓包用了 `9999` = 一次拉全表；skill 里建议按 `100` 起步、必要时翻页)
- Content-Type: `application/json`
- 请求头：仅需 `Cookie`（同前几节）

### 请求体（过滤条件，均为字符串，空串 = 不过滤）

```jsonc
{
  "name": "",        // 环境名精确 / 模糊匹配（服务端行为未验证，见下）
  "hashName": "",    // 按短标识过滤
  "creator": "",     // 按创建者过滤（RTX / 工号，待观察）
  "desc": "",        // 按备注过滤
  "type": ""         // 环境类型：如 "vt"；空 = 全部类型
}
```

**⚠️ 过滤是精确匹配还是模糊匹配未验证**。skill 使用时，先按 `name = <目标环境名>` 精确查一次，若返回 0 条再放宽（例如查 `type=vt` 拉全表本地过滤），避免误判"不存在"。

### 响应（实测 2026-07-13，样本 `_probe_out/envs_query_after_create.json`）

```jsonc
{
  "code": 0,
  "message": "OK",
  "data": {
    "page": 1,
    "pageSize": 100,
    "count": 1,              // ⭐ 总数（用于翻页 / 判断"是否存在"）。0 = 无匹配
    "list": [                // ⭐ 数组字段名是 "list"（不是 items / envs / records）
      {
        // 单条 env 结构与 §4 响应里的 data 基本一致：
        "id": 6593,
        "name": "cospec-probe-xzh-1783928207",
        "hashName": "7jawpjbngz5rl",
        "type": "vt",
        "desc": "...",
        "projectID": 71,
        "endTime": "2026-07-13T08:36:48Z",         // ⚠️ 秒精度（无毫秒），与 §4 响应里的毫秒精度不一致
        "creator": "kennyzhxu",
        "prevent": false,
        "status": 0,
        "createdAt": "2026-07-13T07:36:49Z",
        "updatedAt": "2026-07-13T07:36:49Z",
        "serviceInstanceList": [ /* 与 §4 响应里的 instance 结构一致，含 status: BUILD_WAIT */ ],
        "envCustom": { "additionEnvVars": [] }
        // ⚠️ 依旧**没有** accessUrl 字段
      }
    ]
  }
}
```

### 关键实测结论

1. **数组字段名是 `list`**（不是 `items` / `envs` / `records`）
2. **分页字段：`page` / `pageSize` / `count`**（`count` = 匹配总数，用它翻页而不是 total）
3. **`name` 过滤是精确匹配** —— 传完整字符串才命中，传子串 `"cospec-probe"` 结果待测（但从 skill 用途看，**精确查刚够用**）
4. **单条 env 结构 = §4 响应里 `data` 的完整回显**，包括 `serviceInstanceList` 和 `envCustom`（不是精简结构），可以直接用于 skill 输出
5. **依旧没有 `accessUrl`** —— 与 §4 一致，需要按 `hashName` 自己拼

### curl 示例（去 cookie）

```bash
curl 'https://zerus.woa.com/api/v4/projects/71/envs_query?page=1&pageSize=100' \
  --http2 \
  -X POST \
  -H 'Content-Type: application/json' \
  -H 'Cookie: P_RIO_TOKEN=<...>; RIO_TOKEN=<...>; x-client-ssid=<...>; x_host_key_access_https=<...>' \
  --data '{"name":"spec-134389917-dashboard-online","hashName":"","creator":"","desc":"","type":"vt"}'
```

完整样例（含 query + body）见 `_probe_out/list_envs_sample_request.json`。

### 错误 & 边界

| 情形 | 处理 |
|------|------|
| `2xx` && `code == 0` | 成功；用 `data.list` 判断同名 |
| `2xx` && `code != 0` | 展示 `message`，**skill 场景下降级为\"跳过防重名检查\"**（不阻塞创建，仅打 warning 提醒用户手动确认）|
| `401` / `302 → 登录页` | Cookie 过期，回 SKILL.md Step 1 重登，重试**一次** |
| `5xx` / 超时 | 指数退避 ≤ 2 次；仍失败**降级跳过**（不阻塞创建，warning 提醒）|

**降级原则**：环境列表接口**不是** skill 的关键路径，任何非 `2xx+code=0` 都**不应该**阻断 Step 4 的创建，只做 warning + 让用户在\"是否继续创建\"的确认里知情。

### 在 skill 里的使用位置

- **Step 3 用户确认前**：按 `name = <草案 env_name>` 查一次；若命中未过期同名（`endTime > now`）：
  - 让用户选：**复用**（返回该环境信息，跳过 Step 4）/ **换名**（回 Step 2 改名）/ **强制新建**（放行 Step 4，接受服务端最终裁决）
- **Step 4 5xx 重试前**：按 `name = <env_name>` 查一次；若已存在且 `creator = 当前用户` 且 `endTime` 合理，则**判定为已创建成功**，跳过重试，直接进入 Step 5 输出
- **Step 5 兜底访问入口**：若 §4 响应没给 `accessUrl`，用本接口按 `hashName` 或 `name` 拉一次，看单条 env 是否带 URL；实在没有就只输出环境名 / ID，让用户去 zerus 控制台开

---

## 6. 修改环境（Update Env）

用途：在已有环境上**改配置**（`name` / `desc` / `endTime` / `prevent` / 附加环境变量）或**增删改服务实例**（新增服务、删掉某个服务、把某个服务切到新分支）。

> ⚠️ **PUT 是全量覆盖语义**：body 就是"环境的完整最新状态"，服务端按新旧 diff 出增删改。**没在 body 里出现的 instance 会被删掉**，忘带任何一项都可能造成误删。安全做法：先 §5 拉现状 → 就地 modify → PUT 回去。

### 请求

- Method: **`PUT`**
- Path: `/api/v4/projects/{project_id}/envs/{env_id}`
  - `project_id`：与前几节一致，当前 `71`
  - `env_id`：来自 §4 响应 `data.id` 或 §5 列表 `data.list[].id`
- Content-Type: `application/json`
- 请求头：仅需 `Cookie`（同前几节）

### 请求体（基于真实抓包 `samples/update_env_request_example.json`）

顶层结构 = §4 请求体 + `id` / `hashName` / `createdAt` / `updatedAt` 回填字段：

```jsonc
{
  "id": 6597,                                    // ⭐ 必填，环境 ID（等于 URL 里的 env_id）
  "name": "test",                                // 可改
  "hashName": "rky8rxxx1z97w",                   // ⭐ 必填，回传原值，服务端不允许改
  "type": "vt",                                  // 必填，保持 "vt"
  "desc": "",                                    // 可改
  "projectID": 71,                               // 必填，回传原值
  "endTime": "2026-07-20T08:43:03Z",             // 可改（延期 / 提前过期）
  "creator": "kennyzhxu",                        // 必填，回传原值
  "prevent": false,                              // 可改（防误删标记）
  "status": 0,                                   // 必填，回传原值
  "createdAt": "2026-07-13T08:43:03Z",           // 必填，回传原值
  "updatedAt": "2026-07-13T08:43:03Z",           // 服务端会重置为本次请求时间
  "serviceInstanceList": [ /* ServiceInstance[] —— **全量列表**，不出现即被删 */ ],
  "envCustom": { "additionEnvVars": [] }         // 可改
}
```

**`serviceInstanceList[]` 的三种项类型**（决定服务端行为）：

| 意图 | `id` / `envID` | 其它字段 | 服务端行为 |
|------|----------------|----------|-----------|
| **保留**（不变） | 非零（如 `id: 15283, envID: 6597`）| 原样回传（含 `buildInfo.bk.projectID/pipelineID/buildID` 等真实值 + `status: "BUILDING"` 等 runtime 字段） | 沿用旧实例，不重新构建 |
| **改分支 / 改参数** | 非零（同上）| 覆盖 `buildInfo.bk.params` 里的 `GIT_BRANCH_PROJECT.defaultValue`（可选 `BK_CI_BUILD_MSG`），其余原样 | 用新参数触发新一轮构建 |
| **新增服务** | **`id: 0` / `envID: 0`** | 按 §4 `serviceInstance` 结构填（`buildInfo.base` / `releaseInfo.base` 均 `{"autoNew": false}`；`bk.params[GIT_BRANCH_PROJECT].auto` 强制 `true` 以开启 push 触发；`bk.projectID/pipelineID/buildID` 全空由服务端补）| 新建一个 instance 并入队（`status: BUILD_WAIT`）|
| **删除服务** | — | **从 list 里省略**（不出现即删）| 旧 instance 被拆除 |

> 抓包样本里的 `palmgrpcuiproxy` 就是**新增**的例子：`id=0/envID=0`、`buildInfo.base = {autoNew: false}`、`releaseInfo` 结构完整；同时保留了 `devicegateway` / `wecarddataanalysisapp` 两条 `id!=0` 的旧实例。

### curl 示例（去 cookie）

```bash
curl 'https://zerus.woa.com/api/v4/projects/71/envs/6597' \
  --http2 \
  -X PUT \
  -H 'Content-Type: application/json' \
  -H 'Cookie: P_RIO_TOKEN=<...>; RIO_TOKEN=<...>; x-client-ssid=<...>; x_host_key_access_https=<...>' \
  --data @update_env_body.json
```

完整请求体样例见 `samples/update_env_request_example.json`。

### 响应（结构推断）

抓包只有 request 侧，服务端响应结构**未实测**。根据 §4 create_env 的规律推断：

```jsonc
{
  "code": 0,
  "message": "OK",
  "data": {
    // = 更新后的完整 env（结构同 §4 响应 data / §5 list[i]）
    "id": 6597,
    "name": "test",
    "hashName": "rky8rxxx1z97w",
    ...
    "serviceInstanceList": [
      // 保留的 instance：id 不变，可能 status/updatedAt 变
      // 新增的 instance：分配到新的 id，status: BUILD_WAIT
      // 删除的 instance：不出现
    ]
  }
}
```

**待抓一次真实响应回填本段**。skill 端目前的做法是**忽略响应体细节**，只判 `code == 0`；操作后需要最新状态就再调一次 §5。

### 字段填法（skill 视角）

| 位置 | skill 填法 |
|------|-----------|
| URL `env_id` | 必填，来自 §4 响应或 §5 列表 |
| body `id` / `hashName` / `projectID` / `creator` / `status` / `createdAt` | 从 §5 拉到的当前 env 原样回传（**不要**自己造值）|
| body `name` / `desc` / `endTime` / `prevent` / `envCustom` | 需要改就改；不改就沿用原值 |
| body `serviceInstanceList` | 从 §5 拉到的当前列表**就地 modify**，再回传全量：<br>• 保留：原样<br>• 改分支：找到 `name == 目标服务` 的项，覆盖 `buildInfo.bk.params` 里 `GIT_BRANCH_PROJECT.defaultValue`<br>• 新增：`build_service_instance(...)` 产出，追加进列表<br>• 删除：从列表 filter 掉对应项 |

### 关于 `params` 覆盖的具体规则（同 §4）

1. **深拷贝** 当前 instance 的 `buildInfo.bk.params[]`，不要就地改
2. 找到 `id == "GIT_BRANCH_PROJECT"` 的那项，把 `defaultValue` 改为新分支名
3. 可选：找到 `id == "BK_CI_BUILD_MSG"` 的那项，改为 `"[cospec] update env for <branch>"`
4. **不要动** `ZERUS` / `elementAtomCode` / `repositoryHashID` / `aliasName` / `url` / `auto` 等
5. 其他参数**一个字段都别动**，原样回传

### 错误 & 边界

| 情形 | 处理 |
|------|------|
| `2xx` && `code == 0` | 成功；建议再拉一次 §5 拿最新状态 |
| `2xx` && `code != 0` | 展示 `message`，**不重试**；常见：分支解析不到、instance id 不属于该 env |
| `400` / `422` | 请求体字段错，把 body diff 打到日志（**必须脱敏 cookie**），交回用户 |
| `401` / `302 → 登录页` | Cookie 过期，回 SKILL.md Step 1 重登，重试**一次** |
| `403` | 无权限（可能不是 env creator）；停止，交回用户 |
| `404` | `env_id` 不存在或已被删；停止，交回用户 |
| `409`（推测）| 状态冲突（如别人在同时改）；提示用户重拉 §5 后再来一次 |
| `5xx` / 超时 | 指数退避重试 ≤ 2 次（2s / 5s）。**⚠️ 全量 PUT 幂等**（重复 PUT 同 body 结果一致），可以放心重试 |

### 幂等性 & 全量覆盖注意

- ✅ **幂等**：同 body 重复 PUT 结果相同（服务端只要看到 body 状态就当前状态即完成）
- ⚠️ **全量语义**：**必须**先 §5 拉到当前完整 env 再改，否则漏带任何 instance 会造成误删；skill 不允许"只传要改的字段"
- ⚠️ **runtime 字段**：保留的 instance 里的 `buildInfo.bk.projectID/pipelineID/buildID` / `status: "BUILDING"` 等**必须原样回传**（服务端可能用它们判断是否是同一次构建）

### 在 skill 里的使用位置

本 skill 主流程（创建环境）**不使用**本接口。以下场景需要用：

- **加服务**：spec 后期发现漏了某个仓库，需要往已有 env 里补一个 service（`build_service_instance(...)` 追加进 list）
- **改分支**：某个 service 换到新的 feature 分支（找到 instance → 覆盖 GIT_BRANCH_PROJECT）
- **减服务**：某个 service 不再需要覆盖分支（回归 baseline），从 list 里删除
- **延期**：`endTime` 快到了，用户想延长（改顶层 `endTime`）
- **改备注**：`desc` 加联调进度说明

推荐流程：

```python
from zerus_api import (
    preflight, get_env_detail, update_env,
    build_service_instance, add_service_instance,
    remove_service_instance_by_name, update_service_branch_by_name,
    get_bk_params, extract_params_list,
)

cookie = preflight()
env = get_env_detail(cookie, env_id=6597)

# 场景 A：加一个服务
params_tpl = extract_params_list(get_bk_params(cookie, 706, 1386))
new_inst = build_service_instance(
    service_id=706, meta_id=1386, service_name="palmgrpcuiproxy",
    branch="feature/xxx", params_template=params_tpl,
)
add_service_instance(env, new_inst)

# 场景 B：删掉一个服务
remove_service_instance_by_name(env, "devicegateway")

# 场景 C：把某个服务切分支
update_service_branch_by_name(env, "wecarddataanalysisapp", "feature/yyy")

# 一把 PUT 回去
resp = update_env(cookie, 6597, env)
assert resp.get("code") == 0
```

---

## 7. 删除环境（Delete Env）

用途：**回收**一个不再需要的联调环境。删除后 zerus 会拆掉所有 instance 的部署，
构建历史/发布记录都会作废，**不可撤销**。

> ⚠️ **危险操作**：skill 在调这个接口前**必须**跟用户二次确认（展示环境名 /
> hashName / 服务列表 / creator），任何自动化流程都不允许静默调用。

### 请求

- Method: **`DELETE`**
- Path: `/api/v4/projects/{project_id}/envs/{env_id}`
  - `project_id`：与前几节一致，当前 `71`
  - `env_id`：来自 §4 响应 `data.id` 或 §5 列表 `data.list[].id`
- 请求头：仅需 `Cookie`（同前几节）
- **请求体**：**空**（原始抓包是 `--data ''` + 默认的
  `Content-Type: application/x-www-form-urlencoded`；服务端只看 URL 里的 env id，
  不读 body）

### curl 示例（去 cookie）

```bash
curl 'https://zerus.woa.com/api/v4/projects/71/envs/6597' \
  --http2 \
  -X DELETE \
  -H 'Cookie: P_RIO_TOKEN=<...>; RIO_TOKEN=<...>; x-client-ssid=<...>; x_host_key_access_https=<...>' \
  --data ''
```

真实抓包脱敏后见 `samples/delete_env_request_example.md`。

### 响应（结构推断 —— 未实测）

抓包只覆盖了 request 侧。按 §4 / §5 / §6 规律推断：

```jsonc
{
  "code": 0,           // 0 = 成功
  "message": "OK",
  "data": null         // 或 { "id": 6597 } —— 未实测
}
```

**skill 端只把 `code == 0` 视为成功**；响应体的具体字段名不做依赖。

**待抓一次真实响应回填本段**（尤其是 `data` 结构 / 错误 message 的原文）。

### 错误 & 边界

| 情形 | 处理 |
|------|------|
| `2xx` && `code == 0` | 成功；建议立刻 §5 拉一次，验证该 env 已从列表消失 |
| `2xx` && `code != 0` | 展示 `message`，**不重试**；常见：`env not found`（已被删）/ `permission denied`（不是 creator） |
| `401` / `302 → 登录页` | Cookie 过期，回 SKILL.md Step 1 重登，重试**一次** |
| `403` | 无权限（可能不是 env creator）；停止，交回用户 |
| `404` | `env_id` 不存在或已被删；给用户"可能已删"的提示后停止 |
| `409` / `423`（推测）| env 处于不可删状态（如 `prevent: true` 防误删标记，或还有构建在跑）；提示用户先解除 |
| `5xx` / 超时 | ⚠️ 由于**推测非幂等**：不做自动重试；提示用户手动确认后再来一次 |

### 幂等性

**推测非幂等**：

- 第一次 DELETE：`code == 0`，环境被删
- 第二次 DELETE 同 env：`code != 0` + `message: env not found`（或类似）
- 网络层重试要**谨慎**：如果第一次请求实际到达服务端但客户端超时，重试会拿到
  `env not found` 但环境实际已被删；skill 端**遇到网络失败不自动重试**，把结果
  和"可能已成功，请到 zerus 控制台确认"一并交回用户

### 前置检查（skill 视角）

调用 `delete_env()` 前**必须**做的检查：

1. **拉一次 §5** 拿到当前 env 的完整信息（name / hashName / creator / serviceInstanceList / prevent）
2. 如果 `prevent: true`（防误删标记），**直接拒绝**并提示用户先在控制台解除
3. 检查 `creator` 是否等于当前用户；不是本人 creator 的环境**默认拒绝**
   （403 也会兜住，但 skill 层提前挡住体验更好）
4. **展示给用户**并等待明确的"确认删除"回答，禁止基于隐式意图（如"清理一下环境"）直接调用
5. 用户确认后再调 `delete_env(cookie, env_id)`

### 在 skill 里的使用位置

本 skill 主流程（创建环境）**不使用**本接口。以下场景需要用：

- **联调结束**：spec 已合入 develop，环境用完想回收
- **建错了**：刚建的 env 服务/分支填错了，想删掉重建
- **清理过期**：`endTime` 已过但服务端还没自动回收，用户想手动清

推荐流程：

```python
from zerus_api import (
    preflight, get_env_detail, delete_env, query_envs, extract_env_list,
)

cookie = preflight()

# 1. 先拉现状，确认这真的是要删的那个
env = get_env_detail(cookie, env_id=6597)
print(f"即将删除：id={env['id']} name={env['name']} creator={env['creator']}")
print(f"服务列表：{[x['name'] for x in env.get('serviceInstanceList') or []]}")

# 2. skill 端跟用户二次确认（略）

# 3. 检查防误删标记
if env.get("prevent"):
    raise SystemExit("[!] env 打了 prevent=true 防误删标记，请先在控制台解除")

# 4. 执行删除
resp = delete_env(cookie, env["id"])
assert resp.get("code") == 0, f"删除失败：{resp.get('message')}"

# 5. 验证：拉一次 §5，确认列表里已经没有它
envs = extract_env_list(query_envs(cookie))
assert not any(str(x.get("id")) == str(env["id"]) for x in envs), "服务端似乎没删掉？"
print("[ok] env 已回收")
```
