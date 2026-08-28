# 前端 评审依据（Frontend Review Guide）

> 适用：Web 前端（`palm_saas_fe` 管控管理端、`palm-wepay/web`、`PaymaxPalmSdk/web`）。
> 技术栈：Vue3 SFC + `<script setup>` + Vite + pinia；`palm_saas_fe` 为**微前端**（pnpm + lerna monorepo，主应用 `packages/main` + 子应用 `packages/iot`/`packages/pay`）；`node >=18 <20`、`pnpm >=6 <10`。
> 加载时机：MR 命中上述前端目录时，Step 4.1 / 4.5 / 4.6 前读完本文件。

---

## 1. 架构与约定（评审基准）

`palm_saas_fe` 目录结构：
```
common/            跨应用公共资源/组件/hooks/utils/types/config
packages/main/     微前端主应用（api / components / layouts / pages / router / store）
packages/iot/      IoT 设备管理子应用
packages/pay/      支付子应用（主应用域名下自动加载）
scripts/           i18n 分析/提取、Vite 配置生成
conf/              config.json、nginx-web.conf
```
- **微前端边界**：主应用与子应用的公共能力放 `common/`；跨 package 引用要合理（子应用别反向依赖主应用私有实现）。
- **状态管理**：pinia store 按域拆分；避免全局巨型 store。
- **国际化**：`zh-CN` / `en-US` / `ja-JP` 三语，命令 `npm run i18n:find|i18n:keys`。

---

## 2. 评审 Checklist（前端专业维度）

### 2.1 i18n 三语完整性（本端硬性重点）
- [ ] 所有新增文案**必须 zh-CN / en-US / ja-JP 三语都给具体词条**，不可留空、不可写"按已有同步"（对照 doc-process 与 SKILL.md 4.1）。
- [ ] 三语 json 文件中是否**真的加了**所有声明的 key（不是只在 spec 里写，代码里漏）？用 `i18n:keys` 核对。
- [ ] 无硬编码中文/英文字面量散落在模板/JS 里（应走 i18n key）。
- [ ] 日期/数字/货币/时区按 locale 格式化；文案长度差异（日文/英文更长）不撑破布局。

### 2.2 组件与状态
- [ ] 复用 `common/components` 已有组件，而非重造相似组件。
- [ ] props 类型完整（TS），避免 `any`；事件命名规范；v-model 用法正确。
- [ ] pinia store 变更有清晰 action，避免组件直接改 state；异步态（loading/error/empty）有处理。
- [ ] 列表/表格：大数据量分页或虚拟滚动；key 唯一稳定（非 index）。

### 2.3 接口对接（与后台契约）
- [ ] `api/` 层封装：请求/响应类型与后台 webgateway 契约一致（字段名、枚举值、错误码）——对照 [`backend.md`](./backend.md) 2.4 与 [`contract.md`](./contract.md)。
- [ ] 错误码处理：后台返回的 errcode 有对应用户提示（走 i18n），不裸露原始错误。
- [ ] 鉴权：请求带 session/CSRF token（webgateway 侧 CSRF + WebSession + 权限校验）；401/403 有统一拦截与跳登录。
- [ ] 分页/游标参数与后台约定一致；避免前端一次拉全量。

### 2.4 安全（@security_rules，前端重点）
- [ ] **XSS**：`v-html` / 富文本必须转义/白名单；用户输入渲染前 escape。
- [ ] 敏感信息不落前端存储（localStorage 不存 token/密钥明文）；日志/console 不打敏感数据。
- [ ] URL/跳转参数校验，防开放重定向；外链 `rel="noopener"`。
- [ ] 权限：前端按钮/路由级权限只是体验，**真实鉴权在后台**——评审别把权限判断只放前端。

### 2.5 工程与质量
- [ ] `npm run lint:all` / `build:all` 能过；无 `console.log` 遗留、无注释死代码。
- [ ] 路由懒加载、组件按需引入；打包体积无明显劣化。
- [ ] 兼容性：目标浏览器范围；子应用加载失败有降级。
- [ ] 关键交互（表单校验、支付流程）有必要的用例或手测说明。

---

## 3. 前端高频坑

| 坑 | 症状 | 对策 |
|----|------|------|
| i18n 留白/漏 key | 只写 zh-CN，或 spec 声明了 key 但 json 没加 | 三语具体词条 + `i18n:keys` 核对 |
| 权限只在前端判 | 后台接口无鉴权，仅前端隐藏按钮 | 后台补鉴权，前端仅体验层 |
| v-html 未转义 | 直接渲染后台/用户内容 | 转义/白名单，防 XSS |
| 字段/枚举对不上 | 前端写死枚举与后台不一致 | 与 proto/contract 对齐 |
| 重造组件 | 又写一个相似 Dialog/Table | 复用 common/components |

---

## 4. 跨端联调契约（前端视角）

- 与**后台**：webgateway 接口字段/错误码/分页契约，见 [`backend.md`](./backend.md) 与 [`contract.md`](./contract.md)。字段变更要前后端同步 spec。
- 与**设备端**：前端展示的设备状态/属性/OTA 进度枚举，要与设备上报的字段语义一致（[`device.md`](./device.md)）。

---

## 5. 本项目真实代码约定与专项检查（带代码依据）

> 从两套前端真实代码提炼。**先分清评审对象**：`palm_saas_fe`（PC 管控后台，Vue3 + Vite + pinia + **wujie 微前端** + **TDesign PC** + vue-i18n）与 `palm-wepay/web`（设备端 H5，**单体** + tdesign-mobile-vue + JSBridge，四语含繁体）——两者架构/i18n/状态完全不同，**不可套用同一规则**。

### 5.1 微前端（wujie，非 qiankun；`palm_saas_fe/packages/main`）
- [ ] 子应用（`MicroFeApp.IOT/SERVICE/OPERATION/PAY`，`constants.ts`）新增/改造要**同步三处**：`hostMap.ts`（URL 映射，开发域名硬编码）、`router/modules/micro-app.ts`（挂 Layout）、`scripts/generateViteConfig.ts` 的 `allowedHosts`——**极易漏改其一**。
- [ ] 🟠 **主→子状态非响应式**：子应用 `useWujieStore()` 从 `global.$wujie.props` 取的是**快照**（`packages/iot/src/store/modules/wujie.ts`），主应用 `userInfo`/`settingStore` 变更**不会自动同步**；语言等要靠 `WujieVue.bus.$emit/$on` 单独广播（`IotView.vue`/`App.vue`）。评审依赖跨应用状态的改动，确认是否漏了 bus 事件。
- [ ] `App.vue` 里 `bus.$on('language')` 每次挂载都注册、`$off` 移除全部监听——多实例/重复挂载场景要防重复注册。
- [ ] `main.ts` 中 `app.use(WujieVue)` 在 `app.mount()` **之后**（顺序反常）——新增插件注册注意时序，别依赖未生效的插件。

### 5.2 i18n（英文原文即 key，非 key 命名法）
- [ ] 🔴 **key 就是英文句子**：`locales/{zh-CN,en-US,ja-JP}.json` 里如 `"Create User": "创建用户"`，代码 `t('Create User')`。**改文案 = 改 key = 三语文件全改**，极易漏翻/裂开——评审文案改动必查三语是否同步、有无残留旧 key。
- [ ] 🔴 三语必须**全部具体**：`palm_saas_fe` 用 `zh/en/ja` 简写（默认 locale `'en'`）；`palm-wepay/web` 是**四语（含繁体）**。别漏语种、别留空。
- [ ] 长句/带插值的 key（含 `!{'@'}` 等 vue-i18n 转义）改动风险高，核对插值占位在三语里都对齐。
- [ ] 语言切换靠三个 `t-config-provider`（`v-if zh/ja/else`）强制重建 TDesign 组件（`App.vue` 顶部注释解释了这个 hack）——新页面别破坏这个结构。

### 5.3 API 请求封装（`packages/*/src/utils/request.ts` + `common/utils` createAxios）
- [ ] 🔴 **鉴权 = localStorage token + Authorization 头**：`requestInterceptors` 从 `localStorage.getItem(TOKEN_NAME)` 取 JWT 放 `headers.Authorization`；`USER_INFO` 也存 localStorage。评审：**敏感信息别再往 localStorage 塞**；token 失效清理要彻底。
- [ ] 统一响应约定：`code === 0` 为成功、取 `data.data`；否则 `throw Error(code)`。新接口返回结构必须遵守 `{code,data,message}` 包裹，否则 `transformRequestHook` 处理不了。
- [ ] 🔴 **401/403 统一跳登录**：`responseInterceptorsCatch` 里 `401`(session 失效)/`403`(CSRF 校验失败) → `redirectLogin()`（清 `USER_INFO` + 跳 `/login?redirect=`）。评审别在业务里各自处理 401/403 绕过统一拦截；登录接口白名单 `['/cgi/login','/cgi/get-csrf']` 要维护。
- [ ] CSRF：后台 webgateway 侧 CSRF 校验（403 触发重登），前端改鉴权流程要与 `/cgi/get-csrf` 配套；GET 自动加时间戳防缓存（`joinTimestamp`）。
- [ ] `api/services/*` 多为按 proto/后台生成的 service 层——字段/枚举**对齐后台 webgateway 契约**（见 [`contract.md`](./contract.md)），别在页面里写死与后台不一致的枚举。

### 5.4 类型 / 安全 / 工程
- [ ] `as any` / `Recordable`（`request.ts` 里已有）滥用要警惕；`api` 响应类型尽量用生成的 model，别 `any` 穿透。
- [ ] `v-html`：管控后台渲染后台返回内容（如富文本/配置）必须转义或白名单，防存储型 XSS。
- [ ] 权限：路由级/按钮级权限（`store/permission.ts`）只是**体验层**，真实鉴权在后台 webgateway `CheckPermissions`——评审别把权限判断只留在前端。
- [ ] 构建：`conf/config.json`（运行时配置）+ `nginx-web.conf`（部署）+ `scripts/generateViteConfig.ts`；改环境/子应用配置注意三者一致。lint 走 `npm run lint:all`。

---

## 🖊 人工追加评审注意点（Human-Curated Review Points）

> 团队前端专家在此追加专业注意点，skill 评审前端端时**必须逐条加载检查**。格式见 [`README.md`](./README.md) 第 3 节。

<!-- HUMAN-CURATED:START -->
<!-- 在本行下方按格式追加条目，勿删除首尾两个标记 -->

<!-- HUMAN-CURATED:END -->
