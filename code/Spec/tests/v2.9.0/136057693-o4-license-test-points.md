# O4 License 授权功能 — Demo 黑盒自测表

> 纯 Demo 页面视角，不涉及代码、协议、日志。只看屏幕上看到什么、点了什么、发生了什么。

---

## 一、冷启动 — 页面入口

| # | 设备状态 | 开机后 Demo 看到的第一个页面 | ✓/✗ |
|---|---------|---------------------------|------|
| 1 | 全新设备（无 license） | 授权扫码页（"Authorization Module with QR code"） | |
| 2 | 有 license 但已过期 | 授权扫码页（同上） | |
| 3 | 有 license 且未过期、已激活 | Home 刷掌页（"Start" 按钮） | |
| 4 | 有 license 且未过期、未激活 | 激活扫码页（"Please activate the device"） | |

---

## 二、授权扫码页 (P-2) — 页面元素

| # | 检查项 | 预期 | ✓/✗ |
|---|-------|------|------|
| 5 | 标题 | "Authorization Module\nwith QR code"，居中，白色粗体 | |
| 6 | 副标题 | "Use this device's camera to\nscan the QR code."，居中，灰色小字 | |
| 7 | 底部图片 | 扫码引导示意图 | |
| 8 | 左上角设置按钮 | 齿轮图标，点击可进入 Debug/Settings 页 | |
| 9 | Cancel 按钮 | 正常授权（未授权设备）→ **无 Cancel 按钮** | |
| 10 | Cancel 按钮 | 重授权（已授权设备从 Debug 点 Re-authorize 进入）→ **有 Cancel 按钮** | |
| 11 | 页面风格 | 深灰底色(#242424)，与激活扫码页视觉一致 | |

---

## 三、授权成功 — 完整链路

| # | 操作 | 预期 | ✓/✗ |
|---|------|------|------|
| 12 | 在授权扫码页扫一个**有效的 license 二维码** | 扫码页 → Loading 转圈 → 授权成功页 | |
| 13 | 授权成功页内容 | 显示绿色成功图标、"Authorized" 文案、APP ID、Valid Until（格式 YYYY-MM-DD） | |
| 14 | 授权成功页按钮 | **没有任何按钮**（纯展示页） | |
| 15 | 授权成功页停留 | 约 3 秒后自动消失 | |
| 16 | 3 秒后（设备未激活） | 自动跳转到激活扫码页 | |
| 17 | 3 秒后（设备已激活） | 自动跳转到 Home 刷掌页 | |

---

## 四、授权失败 — 各种失败场景

| # | 操作 | 预期看到的失败页内容 | ✓/✗ |
|---|------|---------------------|------|
| 18 | 扫一个**已过期的 license 二维码** | 失败页显示红色图标、"Authorization Failed"、副标题显示具体错误原因（如 "QR code expired"），只有 Retry 按钮 | |
| 19 | 扫一个**内容无效/格式错误的二维码** | 失败页，副标题显示错误原因（如 "invalid timestamp"），只有 Retry 按钮 | |
| 20 | 扫一个**非 license 的二维码**（如支付码、激活码） | 扫码页无任何反应，继续等待扫码 | |
| 21 | 扫码后等待超时（断网等，约 30 秒） | 失败页，副标题显示 "Authorization response timeout, please retry" | |

---

## 五、失败后重试

| # | 操作 | 预期 | ✓/✗ |
|---|------|------|------|
| 22 | 在失败页点 Retry | 回到授权扫码页，可重新扫码 | |
| 23 | 连续扫码失败 3 次 | 每次都能 Retry 回到扫码页重试 | |

---

## 六、从其他页面返回 HomeActivity 时的路由

| # | 操作 | 预期 | ✓/✗ |
|---|------|------|------|
| 24 | 未授权设备，从 Debug/Settings 页返回 | 回到授权扫码页（不是刷掌页） | |
| 25 | 已授权设备，从 Debug/Settings 页返回 | 回到 Home 刷掌页 | |
| 26 | License 已过期，从 Debug/Settings 页返回 | 回到授权扫码页 | |

---

## 七、Debug → Authorization Status 页

| # | 操作 | 预期 | ✓/✗ |
|---|------|------|------|
| 27 | 已授权设备，进入 Debug → Authorization Status | 显示状态为 "Authorized"，下方有 APP ID 和 Valid Until | |
| 28 | 未授权设备，进入 Debug → Authorization Status | 显示状态为 "Unauthorized"，APP ID / Valid Until 为 "N/A"，**无按钮** | |
| 29 | License 已过期，进入 Debug → Authorization Status | 显示状态为 "Expired"，Valid Until 为红色加 "(Expired)" 后缀，**无按钮** | |

---

## 八、Debug → Module Info → 激活

| # | 操作 | 预期 | ✓/✗ |
|---|------|------|------|
| 30 | 未激活设备，Debug → Module info | Activation 行显示 "Not Activated" + "Go to activate" 按钮 | |
| 31 | 点击 Go to activate | 进入激活流程，可扫码激活 | |
| 32 | 已激活设备，Debug → Module info | "Go to activate" 按钮不显示 | |
| 33 | Debug → Module info → Module Time 行 | 显示模组当前系统时间（格式 `yyyy-MM-dd HH:mm:ss` UTC），时间值与模组本地时间一致；模组未返回时间戳时显示 "—" | |

---

## 九、运行时 License 过期

| # | 操作 | 预期 | ✓/✗ |
|---|------|------|------|
| 34 | 在 Home 刷掌页正常使用时，后台 license 突然过期 | 页面自动跳转到授权扫码页 | |

---

## 十、设置按钮入口

| # | 操作 | 预期 | ✓/✗ |
|---|------|------|------|
| 35 | 授权扫码页左上角点 Settings | 跳转到 Settings/Debug 页面 | |
| 36 | Home 刷掌页左上角点 Settings | 跳转到 Settings/Debug 页面 | |

---

## 十一、已授权设备重新授权

| # | 操作 | 预期 | ✓/✗ |
|---|------|------|------|
| 37 | Debug → Authorization Status → Re-authorize | 进入授权扫码页（有 Cancel 按钮） | |
| 38 | 扫有效新 license | 成功页 → 3s 后回到 Authorization Status 页，显示新的 APP ID / Valid Until | |
| 39 | 扫无效码 | 失败页 → Retry 回到扫码页 | |
| 40 | 在扫码页点 Cancel | 回到 Authorization Status 页，旧 license 信息不变 | |

---

## 十二、视觉一致性

| # | 检查项 | 预期 | ✓/✗ |
|---|-------|------|------|
| 41 | 授权扫码页 vs 激活扫码页 | 布局结构、字号、间距、配色一致 | |
| 42 | 成功页 vs 失败页 | 风格统一，区别仅在图标颜色和按钮有无 | |
| 43 | Loading 页 | 有旋转动画，不黑屏 | |

---

## 备注

- 二维码准备：有效 license 码、过期 license 码、无效格式码、非 license 码（激活码/支付码），各 1 个
- 模组需提前烧录对应状态固件
- 标注 ✓ 表示通过，✗ 表示不通过需修复
