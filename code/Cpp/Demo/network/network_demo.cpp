// Copyright © 1998 - 2026 Tencent. All Rights Reserved.

// 通用网络库 v2 能力演示（对标 OkHttp/Retrofit/axios 的封装思路）：
//   1. 链式请求构造（Fluent API）
//   2. 拦截器链：日志 / 公共 Header / 重试 / 签名（业务横切逻辑外置）
//   3. Response<T> 泛型响应（业务结构体直出）
//   4. 文件上传（Multipart）+ 下载
//   5. 超时控制 + 异步（回调 / future）
//
// 内置 mini 服务器，无需外网即可验证。

#include <atomic>
#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>

#include "network/api_client.h"
#include "network/http_client.h"
#include "network/interceptor.h"
#include "network/mini_test_server.h"
#include "nlohmann/json.hpp"

// ---------- 业务结构体：与 palm::entity 约定一致（ToJson / FromJson） ----------
namespace entity {

struct CreateUserReq {
  std::string name;
  int age = 0;
  nlohmann::json ToJson() const { return {{"name", name}, {"age", age}}; }
};

struct CreateUserResp {
  std::string name;
  int age = 0;
  static CreateUserResp FromJson(const nlohmann::json& j) {
    CreateUserResp r;
    if (j.contains("body")) {
      r.name = j["body"].value("name", "");
      r.age = j["body"].value("age", 0);
    }
    return r;
  }
};

struct UploadResp {
  std::string filename;
  size_t size = 0;
  static UploadResp FromJson(const nlohmann::json& j) {
    UploadResp r;
    r.filename = j.value("filename", "");
    r.size = j.value("size", 0);
    return r;
  }
};

}  // namespace entity

// 计数拦截器：放在重试拦截器之后（更靠近网络层），每次真实请求都会 +1，
// 用于验证重试次数（外层日志拦截器只会看到"一次"请求，OkHttp 同理）
class CountingInterceptor : public http::Interceptor {
 public:
  explicit CountingInterceptor(int& count) : count_(count) {}
  http::Response Intercept(http::Chain& chain) override {
    count_++;
    return chain.Proceed();
  }

 private:
  int& count_;
};

int failed = 0;
#define CHECK(cond)                                                                     \
  do {                                                                                  \
    if (cond) {                                                                         \
      std::cout << "  [PASS] " << #cond << std::endl;                                   \
    } else {                                                                            \
      std::cout << "  [FAIL] " << #cond << "  (line " << __LINE__ << ")" << std::endl;  \
      failed++;                                                                          \
    }                                                                                   \
  } while (0)

int main() {
  http::test::MiniTestServer server;
  if (!server.Start()) {
    std::cerr << "mini server start failed" << std::endl;
    return 1;
  }
  std::cout << "[mini server] " << server.base_url() << std::endl;

  // ============================================================
  // 1. 链式请求构造（requests/axios 风格）
  // ============================================================
  std::cout << "\n=== 1. 链式构造 Fluent API ===" << std::endl;
  http::HttpClient client(server.base_url());
  auto r1 = client.Get("/get")
                .Query({{"id", "42"}, {"verbose", "true"}})
                .Header("X-Client", "demo")
                .Timeout(std::chrono::seconds(5))
                .Send();
  auto j1 = nlohmann::json::parse(r1.body);
  CHECK(r1.Ok());
  CHECK(j1["query"]["id"] == "42");

  auto r2 = client.Post("/echo")
                .Body(R"({"name":"robot","ver":3})")
                .Header("X-Trace", "abc-123")
                .Send();
  auto j2 = nlohmann::json::parse(r2.body);
  CHECK(r2.Ok());
  CHECK(j2["body"]["name"] == "robot");

  // ============================================================
  // 2. 拦截器链（OkHttp 风格）
  // ============================================================
  std::cout << "\n=== 2. 拦截器链 ===" << std::endl;
  std::vector<std::string> logs;
  http::ClientConfig cfg;
  cfg.logger = [&](const std::string& s) { logs.push_back(s); };
  cfg.enable_logging = true;
  cfg.retry = http::RetryPolicy{3, std::chrono::milliseconds(50)};  // 最多 3 次，50ms 起退避

  http::HttpClient client2(server.base_url(), cfg);
  // 公共 header（UA/trace 等横切信息不再散落在业务代码里）
  client2.AddInterceptor(
      std::make_shared<http::interceptor::HeaderInterceptor>(http::Headers{{"X-App", "palm"}}));
  client2.AddInterceptor(std::make_shared<http::interceptor::BearerAuthInterceptor>("tk-9527"));
  // 签名拦截器：把原 palm 工程"三种签名"这类逻辑外置
  client2.AddInterceptor(std::make_shared<http::interceptor::SigningInterceptor>(
      [](const std::string& method, const std::string& path, const std::string& body) {
        return "SIGN(" + method + "," + path + "," + std::to_string(body.size()) + ")";
      }));

  auto r3 = client2.Get("/get").Query({{"auth", "1"}}).Send();
  CHECK(r3.Ok());
  CHECK(logs.size() >= 2);  // 请求 + 响应两条日志
  std::cout << "  日志样例: " << (logs.empty() ? "" : logs.front()) << std::endl;

  // 重试验证：/status/500 会被重试到 3 次（计数拦截器在最内层，能数到全部尝试）
  int attempts = 0;
  client2.AddInterceptor(std::make_shared<CountingInterceptor>(attempts));
  size_t before = logs.size();
  client2.Get("/status/500").Send();
  size_t log_added = logs.size() - before;
  CHECK(attempts == 3);     // 真实请求 3 次
  CHECK(log_added == 2);    // 外层日志只呈现 1 组（请求 + 响应），重试对它透明
  std::cout << "  真实请求次数=" << attempts << "，外层日志条数=" << log_added
            << "（重试对最外层透明）" << std::endl;

  // ============================================================
  // 3. Response<T> 泛型响应（Retrofit 风格）
  // ============================================================
  std::cout << "\n=== 3. Response<T> 泛型响应 ===" << std::endl;
  api::ApiClient api(server.base_url());
  entity::CreateUserReq req{"xiaoming", 18};
  auto resp = api.Post<entity::CreateUserResp>("/echo", req);
  CHECK(resp.IsSuccess());
  CHECK(resp.Data().name == "xiaoming");
  CHECK(resp.Data().age == 18);
  std::cout << "  Post<CreateUserResp> -> name=" << resp.Data().name
            << " age=" << resp.Data().age << std::endl;

  // 失败场景：HTTP 500 → IsSuccess()==false，error 有值
  auto bad = api.Get<entity::CreateUserResp>("/status/500");
  CHECK(!bad.IsSuccess());
  CHECK(bad.status_code == 500);

  // ============================================================
  // 4. 文件上传（Multipart）+ 下载
  // ============================================================
  std::cout << "\n=== 4. 上传 / 下载 ===" << std::endl;
  {
    std::ofstream("/tmp/netclient_upload.txt") << "hello-palm";  // 构造待上传文件
  }
  auto up = api.Upload("/upload", "file", "/tmp/netclient_upload.txt");
  CHECK(up.Ok());
  CHECK(up.data["filename"] == "netclient_upload.txt");
  std::cout << "  upload -> " << up.data.dump() << std::endl;

  // 下载：/get 的响应体存为文件
  auto dl = api.Download("/get", "/tmp/netclient_download.json");
  CHECK(dl.Ok());
  {
    std::ifstream in("/tmp/netclient_download.json");
    std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    CHECK(!content.empty());
  }

  // ============================================================
  // 5. 超时控制
  // ============================================================
  std::cout << "\n=== 5. 超时控制 ===" << std::endl;
  auto slow = client.Get("/slow").Query({{"ms", "800"}}).Timeout(std::chrono::milliseconds(200)).Send();
  CHECK(!slow.Ok());              // 200ms 超时，服务端 800ms 才响应
  CHECK(slow.status_code == 0);   // 网络层失败
  std::cout << "  timeout error: " << slow.error << std::endl;

  // ============================================================
  // 6. 异步：回调 + future
  // ============================================================
  std::cout << "\n=== 6. 异步（回调 / future） ===" << std::endl;
  std::atomic<int> cb_count{0};
  client.Get("/get").Query({{"async", "cb"}}).SendAsync([&](http::Response r) {
    if (r.Ok()) cb_count++;
  });
  for (int i = 0; i < 100 && cb_count == 0; i++)
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
  CHECK(cb_count == 1);

  auto fut = client.Post("/echo").Body(R"({"future":true})").SendAsync();
  auto fr = fut.get();  // 阻塞等待结果
  CHECK(fr.Ok());
  CHECK(nlohmann::json::parse(fr.body)["body"]["future"] == true);

  // ============================================================
  std::cout << "\n========== " << (failed == 0 ? "ALL PASS" : "FAILED") << " ==========" << std::endl;
  server.Stop();
  return failed == 0 ? 0 : 1;
}
