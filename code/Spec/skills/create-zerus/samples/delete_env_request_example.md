# §7 delete_env 真实抓包（脱敏）

来源：2026-07-13 用户提供的浏览器抓包。cookie 值已经用占位替换，禁止把真实
cookie 提交到仓库。

- 项目：`71`（palm）
- 环境：`6597`（`hashName=rky8rxxx1z97w`，`name=test`）
- 请求体：**空**（`--data ''`，虽然带了 `Content-Type: application/x-www-form-urlencoded`
  但没有任何 body 字段；服务端只看 URL 里的 env id）

```bash
curl --location --request DELETE 'https://zerus.woa.com/api/v4/projects/71/envs/6597' \
  --http2 \
  -H 'Cookie: RIO_TOKEN=<RIO_TOKEN>; x-client-ssid=<SSID>; x_host_key_access_https=<HOST_KEY>; P_RIO_TOKEN=<P_RIO_TOKEN>' \
  --data ''
```

## 响应

**未抓到**（用户只提供了 request 侧）。按 §4/§5/§6 规律推断为：

```jsonc
{ "code": 0, "message": "OK", "data": null /* 或 { "id": 6597 } */ }
```

`code != 0` 时 `message` 会带原因（如 `env not found` / `permission denied`）。

## 幂等性

**推测幂等**（对同一 env 多次 DELETE 第二次会 `code != 0` + `env not found`，
但不会返回 5xx）；skill 端把 `code == 0` 视为唯一成功条件，重复删触发的
"already deleted" 类错误应给用户明确提示，不做自动重试。
