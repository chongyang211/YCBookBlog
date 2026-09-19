// Copyright © 1998 - 2026 Tencent. All Rights Reserved.

// 通用网络库用法示例：
//   1. 启动内置 mini 服务器（无外网也能验证）
//   2. HttpClient：GET / POST / 重试 / 异步
//   3. ApiClient：JSON 化调用 + 泛型反序列化（与 palm::entity 的 ToJson/FromJson 约定兼容）

#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>

#include "network/api_client.h"
#include "network/http_client.h"
#include "network/mini_test_server.h"
#include "nlohmann/json.hpp"

// ---------- 演示用业务结构体：与 palm::entity 约定完全一致 ----------

namespace entity {

struct CreateUserReq {
  std::string name;
  int age = 0;

  nlohmann::json ToJson() const {
    return {{"name", name}, {"age", age}};
  }
};

struct CreateUserResp {
  std::string name;
  int age = 0;
  std::string method;

  static CreateUserResp FromJson(const nlohmann::json& j) {
    CreateUserResp r;
    if (j.contains("body") && j["body"].is_object()) {
      r.name = j["body"].value("name", "");
      r.age = j["body"].value("age", 0);
    }
    r.method = j.value("method", "");
    return r;
  }
};

}  // namespace entity

#define CHECK(cond)                                                                 \
  do {                                                                              \
    if (cond) {                                                                     \
      std::cout << "  [PASS] " << #cond << std::endl;                                \
    } else {                                                                        \
      std::cout << "  [FAIL] " << #cond << "  (line " << __LINE__ << ")" << std::endl; \
      failed++;                                                                     \
    }                                                                               \
  } while (0)

int main() {
  int failed = 0;

  // ============================================================
  // 0. 启动内置测试服务器（127.0.0.1，系统分配空闲端口）
  // ============================================================
  http::test::MiniTestServer server;
  if (!server.Start()) {
    std::cerr << "mini server start failed" << std::endl;
    return 1;
  }
  std::cout << "[mini server] listening on " << server.base_url() << std::endl;

  // ============================================================
  // 1. HttpClient：通用 GET/POST（原始层）
  // ============================================================
  std::cout << "\n=== 1. HttpClient 同步 GET/POST ===" << std::endl;
  http::HttpClient http(server.base_url());
  http.SetDefaultHeader("User-Agent", "netclient-demo/1.0");

  // GET + 查询参数
  auto r1 = http.Get("/get", {{"id", "42"}, {"verbose", "true"}});
  std::cout << "GET /get -> status=" << r1.status_code << " elapsed=" << r1.elapsed_ms << "ms"
            << std::endl;
  auto j1 = nlohmann::json::parse(r1.body);
  CHECK(r1.Ok());
  CHECK(j1["query"]["id"] == "42");

  // POST JSON
  auto r2 = http.Post("/echo", R"({"hello":"world","n":123})");
  auto j2 = nlohmann::json::parse(r2.body);
  CHECK(r2.Ok());
  CHECK(j2["body"]["hello"] == "world");

  // 404 处理
  auto r3 = http.Get("/not-exist");
  CHECK(r3.status_code == 404);
  CHECK(!r3.Ok());

  // 5xx 自动重试（重试 1 次，两次都 500，最终返回 500）
  http::RequestConfig& cfg = http.config();
  cfg.max_retries = 1;
  cfg.retry_interval_ms = 50;
  auto r4 = http.Get("/status/500");
  CHECK(r4.status_code == 500);
  cfg.max_retries = 0;  // 恢复，避免影响后续演示

  // ============================================================
  // 2. ApiClient：JSON 化调用（推荐日常使用）
  // ============================================================
  std::cout << "\n=== 2. ApiClient JSON 调用 ===" << std::endl;
  api::ApiClient api(server.base_url());
  api.SetDefaultHeader("X-Client", "demo");

  // GET：直接拿 JSON
  auto a1 = api.GetJson("/get", {{"kw", "robot"}});
  CHECK(a1.Ok());
  CHECK(a1.data["query"]["kw"] == "robot");
  std::cout << "  GetJson data: " << a1.data.dump() << std::endl;

  // POST：传 nlohmann::json，无需手写 dump
  auto a2 = api.PostJson("/echo", {{"device", "palm"}, {"ver", 3}});
  CHECK(a2.Ok());
  CHECK(a2.data["body"]["device"] == "palm");

  // PUT / DELETE
  CHECK(api.PutJson("/echo", {{"k", "v"}}).Ok());
  CHECK(api.DeleteJson("/get").Ok());

  // 非 JSON 原始 POST（表单）
  auto a3 = api.PostRaw("/echo", "name=foo&age=1", "application/x-www-form-urlencoded");
  CHECK(a3.Ok());

  // ============================================================
  // 3. 泛型调用：业务结构体直进直出（对齐 palm::entity 约定）
  // ============================================================
  std::cout << "\n=== 3. 泛型 API 调用（entity 直进直出） ===" << std::endl;
  entity::CreateUserReq req{"xiaoming", 18};
  entity::CreateUserResp resp;
  bool ok = api.Post("/echo", req, resp);
  CHECK(ok);
  CHECK(resp.name == "xiaoming");
  CHECK(resp.age == 18);
  std::cout << "  Post<CreateUserReq, CreateUserResp> -> name=" << resp.name
            << " age=" << resp.age << std::endl;

  // ============================================================
  // 4. 异步请求（回调在 cpr 线程池执行）
  // ============================================================
  std::cout << "\n=== 4. 异步 GET（回调） ===" << std::endl;
  std::atomic<int> done{0};
  http.GetAsync("/get", {{"async", "1"}}, {}, [&](http::HttpResult r) {
    std::cout << "  [async callback] status=" << r.status_code << std::endl;
    done++;
  });
  // 等待回调完成（demo 场景简单轮询）
  for (int i = 0; i < 100 && done == 0; i++) std::this_thread::sleep_for(std::chrono::milliseconds(20));
  CHECK(done == 1);

  // ============================================================
  // 5. 错误处理演示：网络失败（连不上的端口）
  // ============================================================
  std::cout << "\n=== 5. 网络失败处理 ===" << std::endl;
  api::ApiClient dead_client("http://127.0.0.1:1");  // 1 端口基本不可达
  dead_client.config().timeout = std::chrono::seconds(1);
  dead_client.config().connect_timeout = std::chrono::seconds(1);
  auto a5 = dead_client.GetJson("/get");
  CHECK(!a5.Ok());
  CHECK(a5.status_code == 0);  // 网络失败，HTTP 状态码为 0
  std::cout << "  network error: " << a5.error << std::endl;

  // ============================================================
  std::cout << "\n========== " << (failed == 0 ? "ALL PASS" : "FAILED") << " ==========" << std::endl;
  server.Stop();
  return failed == 0 ? 0 : 1;
}
