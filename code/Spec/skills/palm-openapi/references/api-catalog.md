# API 接口目录

> 📝 本目录持续补充中，当前已覆盖高频接口（编号 1-7、14）的详细参数说明，其余接口将在后续版本中逐步补全。

## 录掌业务域

| 编号 | 接口名称 | URI | 方法 | 说明 |
|------|---------|-----|------|------|
| 1 | 获取录掌二维码 | `/cgi-bin/user-palm-register` | POST | 创建用户记录并生成录掌二维码 |
| 2 | 查询录掌结果 | `/cgi-bin/check-palm-record-result` | POST | 轮询录掌结果（0-待处理/1-成功/2-失败） |
| 3 | 查询录掌状态 | `/cgi-bin/get-user-palm` | POST | 查询用户录掌状态（Registered/Unregistered/PreRegistered） |
| 4 | 删除掌纹 | `/cgi-bin/delete-palm` | POST | 解绑用户掌纹信息 |
| 19 | 申请掌纹录入 | `/cgi-bin/apply-palm-register` | POST | 申请掌纹录入 |

## 查询业务域

| 编号 | 接口名称 | URI | 方法 | 说明 |
|------|---------|-----|------|------|
| 6 | 查询刷掌记录 | `/cgi-bin/pass-records/list` | POST | 拉取刷掌记录列表，支持按用户/商户/时间过滤 |

## 推送业务域

| 编号 | 接口名称 | URI | 方法 | 说明 |
|------|---------|-----|------|------|
| 5 | 掌纹状态变更通知 | 客户提供地址 | POST | 用户掌纹状态变化异步通知（录掌/删除） |
| 14 | 刷掌记录通知 | 客户提供地址 | POST | 用户刷掌成功时推送事件详情 |

## 商户业务域

| 编号 | 接口名称 | URI | 方法 | 说明 |
|------|---------|-----|------|------|
| 7 | 新建商户 | `/cgi-bin/merchant/create` | POST | 在业务线下创建商户 |
| 8 | 获取商户 | `/cgi-bin/merchant/get` | POST | 获取商户信息 |
| 9 | 编辑商户 | `/cgi-bin/merchant/update` | POST | 编辑商户信息 |
| 10 | 删除商户 | `/cgi-bin/merchant/delete` | POST | 删除商户 |

## 设备业务域

> ⏳ 接口详情待补充

| 编号 | 接口名称 | URI | 方法 | 说明 |
|------|---------|-----|------|------|
| 11 | 录入设备 | `/cgi-bin/device/create` | POST | 录入设备到商户下 |
| 12 | 获取设备 | `/cgi-bin/device/get` | POST | 获取设备信息 |
| 13 | 编辑设备 | `/cgi-bin/device/update` | POST | 编辑设备信息 |

## 用户业务域

> ⏳ 接口详情待补充

| 编号 | 接口名称 | URI | 方法 | 说明 |
|------|---------|-----|------|------|
| 15 | 创建用户 | `/cgi-bin/users/create` | POST | 创建用户 |
| 16 | 编辑用户 | `/cgi-bin/users/update` | POST | 编辑用户信息 |
| 17 | 删除用户 | `/cgi-bin/users/delete` | POST | 删除用户 |
| 18 | 获取用户信息 | `/cgi-bin/users/get` | POST | 获取用户信息 |

## 用户标签业务域

> ⏳ 接口详情待补充

| 编号 | 接口名称 | URI | 方法 | 说明 |
|------|---------|-----|------|------|
| 20 | 新建标签 | `/cgi-bin/tags/create` | POST | 新建用户标签 |
| 21 | 编辑标签 | `/cgi-bin/tags/update` | POST | 编辑用户标签 |
| 22 | 删除标签 | `/cgi-bin/tags/delete` | POST | 删除用户标签 |

---

## 接口详情

### 1. 获取录掌二维码

**入参：**

| 参数 | 必填 | 类型 | 说明 |
|------|------|------|------|
| user_id | 是 | string[4,64] | 用户唯一标识 |
| user_name | 否 | string[0,32] | 用户名称（需加密） |
| payment_token | 否 | string[1,1024] | 支付票据（需加密） |
| phone_no | 否 | string[4,20] | 手机号（需加密） |
| physical_card_no | 否 | string[0,32] | 物理卡号（需加密） |

**出参：**

| 参数 | 类型 | 说明 |
|------|------|------|
| code | int | 返回码，0=成功 |
| message | string | 返回描述 |
| code_str | string | 录掌二维码字符串 |

**错误码：** 0-成功, 1000-参数错误, 1100-服务内部错误, 10020-用户已注册, 100027-掌库配额已满

---

### 2. 查询录掌结果

**入参：**

| 参数 | 必填 | 类型 | 说明 |
|------|------|------|------|
| code_str | 是 | string | 录掌二维码 |

**出参：**

| 参数 | 类型 | 说明 |
|------|------|------|
| code | int | 返回码，0=成功 |
| message | string | 返回描述 |
| result | int | 录掌结果：0-待处理, 1-成功, 2-失败 |
| fail_reason | string | 录掌失败原因 |
| palm_direction | int | 录掌方向：0-无效, 1-左手, 2-右手 |
| physical_card_no | string | 物理卡号 |

**错误码：** 0-成功, 1000-参数错误, 1100-服务内部错误, 10007-二维码无效或过期, 100027-掌库配额已满

---

### 3. 查询录掌状态

**入参：**

| 参数 | 必填 | 类型 | 说明 |
|------|------|------|------|
| user_id | 是 | string | 用户唯一标识 |

**出参：**

| 参数 | 类型 | 说明 |
|------|------|------|
| code | int | 返回码，0=成功 |
| message | string | 返回描述 |
| palm_state | string | Registered-已录入, Unregistered-未录入, PreRegistered-预录入 |
| palm_direction | int | 录掌方向：0-无效, 1-左手, 2-右手 |

**错误码：** 0-成功, 1000-参数错误, 1100-服务内部错误, 10003-用户不存在

---

### 4. 删除掌纹

**入参：**

| 参数 | 必填 | 类型 | 说明 |
|------|------|------|------|
| user_id | 是 | string | 用户唯一标识 |

**出参：**

| 参数 | 类型 | 说明 |
|------|------|------|
| code | int | 返回码，0=成功 |
| message | string | 返回描述 |

**错误码：** 0-成功, 1000-参数错误, 1100-服务内部错误, 10003-用户不存在, 10021-用户未录掌

---

### 5. 掌纹状态变更推送通知

**推送入参（平台→客户）：**

| 参数 | 必填 | 类型 | 说明 |
|------|------|------|------|
| action | 是 | string | 事件类型："update_palm" |
| user_id | 是 | string[4,64] | 用户唯一标识 |
| palm_state | 是 | string | Registered/Unregistered/PreRegistered |
| palm_direction | 否 | int | 录掌方向：0-无效, 1-左手, 2-右手 |

**客户返回：** code=0 表示接收成功

---

### 6. 查询刷掌记录

**入参：**

| 参数 | 必填 | 类型 | 说明 |
|------|------|------|------|
| pagination.offset | 是 | int | 偏移量 |
| pagination.limit | 是 | int | 限制条数，最大100 |
| filter.user_id | 否 | string | 用户ID |
| filter.out_merchant_id | 否 | string | 商户ID |
| filter.start_time | 否 | int | 开始时间戳 |
| filter.end_time | 否 | int | 结束时间戳 |

**出参：**

| 参数 | 类型 | 说明 |
|------|------|------|
| code | int | 返回码 |
| message | string | 返回描述 |
| list | array | 刷掌记录列表 |
| list[].user_id | string | 用户id |
| list[].user_name | string | 用户名 |
| list[].device_sn | string | 设备sn |
| list[].device_name | string | 设备名 |
| list[].palm_time | int | 刷掌时间戳 |
| list[].out_merchant_id | string | 商户id |
| list[].out_merchant_name | string | 商户名称 |
| list[].palm_direction | int | 录掌方向 |
| list[].tags | array[string] | 用户标签列表 |

---

### 7. 新建商户

**入参：**

| 参数 | 必填 | 类型 | 说明 |
|------|------|------|------|
| out_merchant_id | 是 | string[1,36] | 商户id |
| out_merchant_name | 是 | string[1,36] | 商户名称 |
| category_id | 是 | string[36,36] | 商户分类id |

**出参：**

| 参数 | 类型 | 说明 |
|------|------|------|
| code | int | 返回码 |
| message | string | 返回描述 |
| data.out_merchant_id | string | 商户id |
| data.out_merchant_name | string | 商户名称 |
| data.category_id | string | 商户分类id |
| data.internal_merchant_id | string | 平台商户id |
| data.business_line_id | string | 业务线id |

---

### 14. 刷掌记录通知

**推送入参（平台→客户）：**

| 参数 | 必填 | 类型 | 说明 |
|------|------|------|------|
| user_id | 是 | string | 用户id |
| user_name | 否 | string | 用户名 |
| device_sn | 是 | string | 设备sn |
| palm_time | 是 | int | 刷掌时间戳 |
| business_line_id | 否 | string | 业务线id |
| out_merchant_id | 否 | string | 商户id |
| out_merchant_name | 否 | string | 商户名称 |
| palm_direction | 是 | int | 录掌方向：1-左手, 2-右手 |
| tags | 否 | array[string] | 用户标签列表 |

**客户返回：** code=0 表示接收成功
