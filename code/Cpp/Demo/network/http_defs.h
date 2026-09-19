// 通用网络库 —— 基础定义层：Method / Request / Response / 重试策略 / 拦截器接口。
//
// 这一层不含任何传输实现（不绑定 cpr/curl），只定义"数据契约 + 拦截点"，
// 因此上层可以替换传输实现，拦截器也能独立单测。
//
// 设计参考：
//   OkHttp  —— 拦截器（Interceptor）责任链，横切关注可插拔
//   Retrofit—— Response<T> 泛型响应，业务结构体直出
//   axios   —— 实例级默认配置 + 链式请求构造

#pragma once

#include <chrono>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace http {

enum class Method { kGet, kPost, kPut, kDelete, kPatch };

using Query = std::vector<std::pair<std::string, std::string>>;
using Headers = std::map<std::string, std::string>;
// multipart 字段：<字段名, 本地文件路径>
using MultipartParts = std::vector<std::pair<std::string, std::string>>;

const char* MethodToString(Method m);

/** @brief 请求描述（拦截器可读写它） */
struct Request {
  Method method = Method::kGet;
  std::string url;            // 完整 URL（base_url + path + query）
  std::string path;           // 原始路径（签名类拦截器按 path 计算摘要时使用）
  Query query;
  Headers headers;
  std::string body;
  std::string content_type;   // 为空表示无 body
  MultipartParts multipart;   // 非空则按 multipart/form-data 发送（文件上传）
  std::chrono::milliseconds timeout{std::chrono::seconds(15)};
  std::chrono::milliseconds connect_timeout{std::chrono::seconds(5)};
  bool verify_ssl = true;
  bool follow_redirects = true;
};

/** @brief 统一响应（原始层） */
struct Response {
  int32_t status_code = 0;   // 网络失败为 0
  std::string body;
  Headers headers;           // 响应头
  std::string final_url;
  std::string error;         // 网络层错误信息
  long elapsed_ms = 0;

  bool Ok() const { return error.empty() && status_code >= 200 && status_code < 300; }
  bool NetworkFailed() const { return !error.empty(); }
};

/**
 * @brief 泛型响应（Retrofit 风格）
 *
 * T 需提供 static T FromJson(const nlohmann::json&)
 */
template <typename T>
struct ResponseT {
  int32_t status_code = 0;
  std::optional<T> data;      // 解析成功时有值
  std::string raw_body;       // 原始响应体（排错用）
  std::string error;          // 网络/HTTP/解析错误
  bool parse_failed = false;

  bool IsSuccess() const { return data.has_value(); }
  const T& Data() const { return *data; }
};

/** @brief 重试策略：仅对“网络失败或 5xx”重试，4xx 是调用方的错，不重试 */
struct RetryPolicy {
  int max_attempts = 1;                                  // 总尝试次数（1 = 不重试）
  std::chrono::milliseconds base_delay{200};             // 首次退避
  int max_delay_ms = 3000;                               // 退避上限（指数增长封顶）
  bool retry_on_5xx = true;                              // 5xx 是否重试
};

/** @brief 可插拔日志（不注入则静默，避免强耦合日志库） */
using Logger = std::function<void(const std::string&)>;

class Chain;

/**
 * @brief 拦截器接口（OkHttp 风格）
 *
 * 典型用途：日志、鉴权、签名、重试、埋点、mock。
 * 处理原则：
 *   - 想改请求：修改 chain.request() 后 Proceed（或构造新 Request 传入）
 *   - 想短路：不调用 Proceed，直接返回 Response（如缓存命中/mock）
 *   - 想改响应：Proceed 之后修改返回的 Response
 */
class Interceptor {
 public:
  virtual ~Interceptor() = default;
  virtual Response Intercept(Chain& chain) = 0;
};
using InterceptorPtr = std::shared_ptr<Interceptor>;

/**
 * @brief 拦截器责任链：index 递增，走到末尾执行真实网络请求
 */
class Chain {
 public:
  Chain(Request request, std::vector<InterceptorPtr> interceptors, size_t index,
        std::function<Response(const Request&)> executor)
      : request_(std::move(request)),
        interceptors_(std::move(interceptors)),
        index_(index),
        executor_(std::move(executor)) {}

  Request& request() { return request_; }
  const Request& request() const { return request_; }

  /** @brief 交给下一个拦截器；已是最后一个则发起真实请求 */
  Response Proceed() { return Proceed(request_); }

  /** @brief 用修改后的请求继续（拦截器改写 request 时使用） */
  Response Proceed(Request request);

 private:
  Request request_;
  std::vector<InterceptorPtr> interceptors_;
  size_t index_;
  std::function<Response(const Request&)> executor_;
};

}  // namespace http
