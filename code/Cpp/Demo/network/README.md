# 通用网络库（NetClient）

零业务依赖的 C++ HTTP 客户端，基于 [cpr](https://github.com/libcpr/cpr) + [nlohmann/json](https://github.com/nlohmann/json)。

设计借鉴三款知名库：

| 来源 | 借鉴点 |
|---|---|
| **OkHttp** | 拦截器（Interceptor）责任链：日志/鉴权/签名/重试等横切关注可插拔 |
| **Retrofit** | `Response<T>` 泛型响应：传 URL + JSON，拿回业务结构体 |
| **axios / requests** | 实例级默认配置 + 链式（Fluent）请求构造 |

## 分层

```
ApiClient    JSON API 层：GetJson/PostJson… + Response<T> 泛型调用 + Upload/Download
HttpClient   通用 HTTP 层：链式构造 + 拦截器链 + 连接复用 + 同步/异步
Interceptor  拦截器：Logging / Header / BearerAuth / Retry / Signing（可自定义）
cpr/curl     传输层（DNS、SSL 会话、TCP 连接跨请求复用）
```

## 构建与运行

使用案例统一在 `Demo/main.cpp`（`./build/demo network`），入口：

```bash
cd code/Cpp/Demo
cmake -B build -DCMAKE_BUILD_TYPE=Release   # 首次 FetchContent 拉取 cpr/json
cmake --build build -j
./build/demo              # 线程库 + 网络库全部演示
./build/demo network      # 只看网络库（内置 mini 服务器，无需外网：ALL PASS）
./build/demo thread       # 只看线程库
```

## 1. 链式请求构造

```cpp
http::HttpClient client("https://api.example.com");

auto r = client.Get("/v1/user")
               .Query({{"id", "42"}})
               .Header("X-Trace", "abc")
               .Timeout(std::chrono::seconds(3))
               .Send();

if (r.Ok()) { /* r.body / r.status_code / r.headers / r.elapsed_ms */ }
else if (r.NetworkFailed()) { /* r.error：连不上/超时 */ }
else { /* 4xx / 5xx */ }
```

## 2. 拦截器链（核心）

```cpp
http::ClientConfig cfg;
cfg.logger = [](const std::string& s) { std::cout << s << std::endl; };
cfg.enable_logging = true;
cfg.retry = http::RetryPolicy{3, std::chrono::milliseconds(200)};  // 最多 3 次，指数退避

http::HttpClient client(base, cfg);
client.AddInterceptor(std::make_shared<http::interceptor::HeaderInterceptor>(
    http::Headers{{"X-App", "palm"}}));
client.AddInterceptor(std::make_shared<http::interceptor::BearerAuthInterceptor>("tk-9527"));

// 业务签名从业务代码里剥离：原 palm 工程"三种签名"就是这种拦截器的实际应用
client.AddInterceptor(std::make_shared<http::interceptor::SigningInterceptor>(
    [](const std::string& m, const std::string& path, const std::string& body) {
      return "SIGN(" + m + "," + path + "," + std::to_string(body.size()) + ")";
    }));
```

自定义拦截器（可实现缓存短路、mock、埋点）：

```cpp
class MyInterceptor : public http::Interceptor {
 public:
  http::Response Intercept(http::Chain& chain) override {
    // 改请求：chain.request().headers["X"] = "y";  return chain.Proceed(chain.request());
    // 短路  ：return http::Response{200, "{\"mock\":1}", {}, "", "", 0};
    // 改响应：auto r = chain.Proceed(); r.body = ...; return r;
    return chain.Proceed();
  }
};
```

> 顺序建议：**日志 → 重试 → 鉴权/签名**。重试放在日志之后，对外层日志只呈现一次请求（与 OkHttp 一致）。

## 3. Response\<T\> 泛型响应

```cpp
api::ApiClient api("https://api.example.com");

// ReqT 需 ToJson() const；RespT 需 static FromJson(const json&)
// ——原 palm::entity 的结构体无需修改即可复用
auto r = api.Post<CreateUserResp>("/v1/user", req);
if (r.IsSuccess()) {
  r.Data().name;             // 已反序列化
} else {
  r.status_code; r.error; r.raw_body; r.parse_failed;
}
```

## 4. 上传 / 下载 / 异步

```cpp
api.Upload("/upload", "file", "/tmp/a.txt");        // Multipart 上传
api.Download("/files/a.bin", "/tmp/a.bin");         // 下载落盘

client.Get("/ping").SendAsync([](http::Response r) { /* 回调 */ });
auto fut = client.Post("/echo").Body("{}").SendAsync();
auto r = fut.get();                                 // future 方式
```

## 错误约定

| 场景 | status_code | error | parse_failed |
|---|---|---|---|
| 2xx + 合法 JSON | 2xx | 空 | false |
| 2xx 但非法 JSON | 2xx | "JSON parse failed…" | true |
| 4xx / 5xx | 404/500… | "HTTP 404" 等 | false |
| 网络失败 / 超时 | **0** | curl 错误信息 | false |

## 文件清单

| 文件 | 说明 |
|---|---|
| `http_defs.h` | Method / Request / Response / RetryPolicy / Chain / Interceptor 接口 |
| `interceptor.h/cpp` | 内置拦截器（Logging/Header/BearerAuth/Retry/Signing）+ 责任链实现 |
| `http_client.h/cpp` | 链式构造器 + 拦截器链 + cpr 执行 + curl share 复用 |
| `api_client.h/cpp` | JSON API 层 + `Response<T>` 泛型 + 上传下载 |
| `mini_test_server.h/cpp` | 迷你测试服务器（`/get` `/echo` `/upload` `/slow` `/status/N`） |
| `network_demo.cpp` | 六大能力演示：21 个断言全绿 |
