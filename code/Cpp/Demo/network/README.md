# 通用网络库（NetClient）

从 palm 工程网络模块中抽取的**零业务依赖**通用层：基于 [cpr](https://github.com/libcpr/cpr)（libcurl 的现代 C++ 封装）+ [nlohmann/json](https://github.com/nlohmann/json)。

> 本目录其余文件（`network.*`、`request_engine.*`、`http_request.*`、`entity/` 等）为原 palm 工程参考代码，依赖 comm/app/device/sm 等内部模块，不在本构建范围。

## 分层

```
ApiClient      JSON API 层：传 URL + JSON 拿回 JSON；泛型调用直连业务结构体
HttpClient     通用 HTTP 层：GET/POST/PUT/DELETE/Download，同步 + 异步回调
cpr/libcurl    传输层
```

## 构建与运行

```bash
cd code/Cpp/Demo/network
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release   # 首次会 FetchContent 拉取 cpr/json
cmake --build build -j
./build/network_demo                              # 内置 mini 服务器，无需外网
```

## 快速上手

### 1. HttpClient（原始层：拿到的是字符串）

```cpp
http::HttpClient client("https://api.example.com");
client.SetDefaultHeader("Authorization", "Bearer <token>");

auto r = client.Get("/v1/user", {{"id", "42"}});
if (r.Ok()) {
  // r.status_code / r.body / r.elapsed_ms / r.final_url
} else if (r.NetworkFailed()) {
  // r.error：连不上 / 超时
} else {
  // r.status_code：4xx / 5xx
}

auto r2 = client.Post("/v1/user", R"({"name":"xx"})");       // JSON body
auto r3 = client.Post("/login", "user=a&pwd=b",
                      "application/x-www-form-urlencoded");   // 任意 Content-Type
auto r4 = client.Download("/files/pkg.bin", "/tmp/pkg.bin"); // 下载落盘

client.GetAsync("/v1/ping", {}, {}, [](http::HttpResult r) { /* cpr 线程池回调 */ });
```

配置（超时 / SSL / 重试）：

```cpp
http::HttpClient client(base, http::RequestConfig{
    .timeout = std::chrono::seconds(10),
    .verify_ssl = false,        // 自签名证书场景
    .max_retries = 2,           // 网络失败/5xx 自动重试（200ms 起指数退避）
});
```

### 2. ApiClient（推荐：JSON 直进直出）

```cpp
api::ApiClient api("https://api.example.com");
api.SetBearerToken("xxx");

auto r = api.GetJson("/v1/user", {{"id", "42"}});
if (r.Ok()) {
  std::string name = r.data["name"];   // 已是 nlohmann::json
} else if (r.parse_failed) {
  // HTTP 200 但响应体不是 JSON，看 r.raw_body
} else {
  // r.status_code / r.error
}

api.PostJson("/v1/user", {{"name", "xx"}, {"age", 18}});
api.PutJson(...); api.DeleteJson(...);
```

### 3. 泛型调用（对齐 palm::entity 的 ToJson/FromJson 约定）

现有 `palm::entity` 的结构体（如 `DeviceHeartbeatReq/Resp`）**无需修改**即可复用：

```cpp
// ReqT 需提供 ToJson() const；RespT 需提供 static FromJson(const json&)
entity::CreateUserReq req{"xiaoming", 18};
entity::CreateUserResp resp;
if (api.Post("/echo", req, resp)) {
  // resp 已完成反序列化
}
```

## HttpResult / ApiResult 错误约定

| 场景 | status_code | error | parse_failed |
|---|---|---|---|
| 2xx + 合法 JSON | 2xx | 空 | false |
| 2xx 但非法 JSON | 2xx | "JSON parse failed: ..." | true |
| 4xx/5xx | 404/500... | "HTTP 404" 等 | false |
| 网络失败/超时 | **0** | curl 错误信息 | false |

## 文件清单

| 文件 | 说明 |
|---|---|
| `http_client.h/cpp` | 通用 HTTP 客户端（同步/异步/重试/下载） |
| `api_client.h/cpp` | JSON API 层（含泛型调用） |
| `mini_test_server.h/cpp` | 迷你测试服务器（127.0.0.1，离线验证用） |
| `network_demo.cpp` | 用法示例：24 个断言全绿 |
| `CMakeLists.txt` | 独立构建（FetchContent 自动拉依赖） |
