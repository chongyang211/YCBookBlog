// 通用 HTTP 客户端 —— 传输层之上的"通用 HTTP 层"
//
// 一次请求的完整链路（从外到内）：
//   RequestBuilder（链式攒参数）
//     → HttpClient::Execute（组装 Request）
//       → Chain（拦截器责任链：日志 → 重试 → 鉴权/签名）
//         → 真实发送（cpr/curl，跨请求复用 DNS / SSL 会话 / TCP 连接）
//       ← Response 沿链回溯，每个拦截器都有机会改写
//     ← Response（或 Response<T>）
//
// 能力：
//   · 链式请求构造（requests/axios 风格）：client.Get("/x").Query(...).Header(...).Send()
//   · 拦截器责任链（OkHttp 风格）：日志/鉴权/签名/重试 可插拔
//   · Response<T> 泛型响应（Retrofit 风格）：业务结构体直出
//   · 跨请求复用 DNS/SSL 会话/连接（curl share，提升短连接性能）
//   · 同步 + 异步（回调 + std::future）
//
// 配置分两级：client 级（拦截器/超时/日志，全实例共享）+ request 级（链式覆盖）。

#pragma once

#include <chrono>
#include <future>
#include <memory>
#include <string>
#include <vector>

#include "network/http_defs.h"

namespace http {

/** @brief 客户端级配置（一个实例的所有请求共享） */
struct ClientConfig {
  std::chrono::milliseconds timeout{std::chrono::seconds(15)};
  std::chrono::milliseconds connect_timeout{std::chrono::seconds(5)};
  bool verify_ssl = true;
  bool follow_redirects = true;
  RetryPolicy retry;                       // 默认不重试（max_attempts=1）
  Logger logger;                           // 为空则静默
  bool enable_logging = false;             // 快捷开关：自动加 LoggingInterceptor
};

class HttpClient;

/**
 * @brief 链式请求构造器（借鉴 requests/axios）
 *
 * 用法：
 *   auto r = client.Get("/v1/user").Query({{"id","1"}}).Header("X-A","b").Timeout(3s).Send();
 */
class RequestBuilder {
 public:
  RequestBuilder(HttpClient* client, Method method, std::string path);

  RequestBuilder& Query(Query q);
  RequestBuilder& Header(const std::string& key, const std::string& value);
  RequestBuilder& Headers(Headers h);
  RequestBuilder& Body(const std::string& body, const std::string& content_type = "application/json");
  RequestBuilder& Json(const std::string& json_str);
  RequestBuilder& Multipart(MultipartParts parts);      // 文件上传
  RequestBuilder& Timeout(std::chrono::milliseconds t);
  RequestBuilder& ConnectTimeout(std::chrono::milliseconds t);
  RequestBuilder& VerifySsl(bool on);

  /** @brief 同步发送（走完整拦截器链） */
  Response Send();

  /**
   * @brief 同步发送并反序列化为 T（T 需提供 static T FromJson(const nlohmann::json&)）
   */
  template <typename T>
  ResponseT<T> SendAs();

  /** @brief 异步发送，回调在 cpr 工作线程执行 */
  void SendAsync(std::function<void(Response)> cb);

  /** @brief 异步发送，返回 std::future（调用方自己决定何时等待） */
  std::future<Response> SendAsync();

 private:
  HttpClient* client_ = nullptr;
  Request req_;
};

/**
 * @brief 通用 HTTP 客户端
 *
 * client 级：拦截器、超时、日志配置；
 * request 级：链式覆盖（Query/Header/Body/Timeout）。
 */
class HttpClient {
 public:
  explicit HttpClient(std::string base_url, ClientConfig cfg = {});
  ~HttpClient() = default;
  HttpClient(const HttpClient&) = delete;
  HttpClient& operator=(const HttpClient&) = delete;

  // ---------- 链式入口 ----------
  RequestBuilder Get(const std::string& path);
  RequestBuilder Post(const std::string& path);
  RequestBuilder Put(const std::string& path);
  RequestBuilder Delete(const std::string& path);
  RequestBuilder Patch(const std::string& path);

  // ---------- 拦截器 ----------
  // 顺序 = 执行顺序；建议：日志 → 重试 → 鉴权/签名 →（末尾为真实请求）
  void AddInterceptor(InterceptorPtr interceptor);
  void ClearInterceptors();

  // ---------- 配置 ----------
  void SetBaseUrl(const std::string& base_url);
  void SetDefaultHeader(const std::string& key, const std::string& value);
  void RemoveDefaultHeader(const std::string& key);
  ClientConfig& config() { return cfg_; }
  const std::string& base_url() const { return base_url_; }
  int port() const { return 0; }  // 保留：便于上层做 mock/测试替换

  // ---------- 便捷同步接口（内部委托给链式构造器，兼容旧用法） ----------
  Response Get(const std::string& path, const Query& query, const Headers& headers = {});
  Response Post(const std::string& path, const std::string& json_body,
                const Headers& headers = {});
  Response Post(const std::string& path, const std::string& body, const std::string& content_type,
                const Headers& headers = {});
  Response Put(const std::string& path, const std::string& json_body, const Headers& headers = {});
  // query 不设默认值：否则单参 Delete(path) 会与链式版 RequestBuilder Delete(path) 歧义
  // 单参数场景请用链式：client.Delete(path).Query(...).Send()
  Response Delete(const std::string& path, const Query& query, const Headers& headers = {});
  Response Download(const std::string& path, const std::string& file_path);

  // ---------- 异步（旧签名，内部委托） ----------
  using Callback = std::function<void(Response)>;
  void GetAsync(const std::string& path, const Query& query, const Headers& headers, Callback cb);
  void PostAsync(const std::string& path, const std::string& json_body, const Headers& headers,
                 Callback cb);

  // ---------- 内部：拦截器链入口 ----------
  Response Execute(Request request);

  // 工具：base_url + path 拼接（path 为完整 URL 时直接用）
  static std::string BuildUrl(const std::string& base_url, const std::string& path);
  static std::string BuildQueryString(const Query& query);

 private:
  friend class RequestBuilder;

  std::string base_url_;
  ClientConfig cfg_;
  Headers default_headers_;
  std::vector<InterceptorPtr> interceptors_;
};

}  // namespace http
