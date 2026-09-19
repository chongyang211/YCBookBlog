// Copyright © 1998 - 2026 Tencent. All Rights Reserved.

// 通用 HTTP 客户端（零业务依赖）：基于 cpr 封装，支持 GET/POST/PUT/DELETE/Download。
// 与原 palm::HttpRequest 的区别：不耦合 Device/Application 等设备逻辑，
// 可用于调用任意 REST API。

#pragma once

#include <chrono>
#include <functional>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace http {

using Query = std::vector<std::pair<std::string, std::string>>;
using Headers = std::map<std::string, std::string>;

/**
 * @brief 统一 HTTP 响应结构
 *
 * status_code == 0 表示请求本身失败（网络错误/超时），此时 error 非空。
 */
struct HttpResult {
  int32_t status_code = 0;   // HTTP 状态码（200/404/500...），网络失败时为 0
  std::string body;          // 响应体（文本）
  std::string error;         // 网络层错误信息，成功为空
  std::string final_url;     // 最终 URL（重定向后）
  long elapsed_ms = 0;       // 请求耗时（毫秒）

  // 2xx 视为业务成功
  bool Ok() const { return error.empty() && status_code >= 200 && status_code < 300; }
  // 网络层失败（连不上/超时/DNS 失败等）
  bool NetworkFailed() const { return !error.empty(); }
};

/**
 * @brief 请求配置（一个 HttpClient 实例共享一组配置）
 */
struct RequestConfig {
  std::chrono::milliseconds timeout{std::chrono::seconds(15)};        // 总超时
  std::chrono::milliseconds connect_timeout{std::chrono::seconds(5)};  // 连接超时
  bool verify_ssl = true;                                              // SSL 证书校验
  bool follow_redirects = true;                                        // 跟随 3xx 重定向
  int max_retries = 0;                                                 // 网络失败/5xx 自动重试次数
  int retry_interval_ms = 200;                                        // 重试间隔（每次翻倍）
};

/**
 * @brief 通用 HTTP 客户端
 *
 * 用法示例：
 *   http::HttpClient client("https://api.example.com");
 *   auto r = client.Get("/v1/user", {{"id", "42"}});
 *   if (r.Ok()) { r.body 就是响应体 }
 */
class HttpClient {
 public:
  /**
   * @param base_url 服务基地址，如 "https://api.example.com"（末尾带不带 / 均可）
   * @param cfg      请求配置（超时/SSL/重试等）
   */
  explicit HttpClient(std::string base_url, RequestConfig cfg = {});

  ~HttpClient() = default;
  HttpClient(const HttpClient&) = delete;
  HttpClient& operator=(const HttpClient&) = delete;

  // ---------- 配置管理 ----------

  void SetBaseUrl(const std::string& base_url);
  // 设置公共 header（本实例所有请求都会带上，如 Authorization/User-Agent）
  void SetDefaultHeader(const std::string& key, const std::string& value);
  void RemoveDefaultHeader(const std::string& key);
  RequestConfig& config() { return cfg_; }

  // ---------- 同步请求 ----------

  /**
   * @brief GET 请求
   * @param path    请求路径，如 "/v1/user"（开头带不带 / 均可）
   * @param query   查询参数（自动 URL 编码），如 {{"id","42"},{"verbose","true"}}
   * @param headers 本次请求附加 header（与公共 header 合并，同名覆盖）
   */
  HttpResult Get(const std::string& path, const Query& query = {}, const Headers& headers = {});

  /**
   * @brief POST JSON 请求（Content-Type: application/json）
   * @param body JSON 字符串
   */
  HttpResult Post(const std::string& path, const std::string& json_body,
                  const Headers& headers = {});

  /**
   * @brief POST 原始请求体（自定义 Content-Type，如 x-www-form-urlencoded、protobuf）
   */
  HttpResult Post(const std::string& path, const std::string& body,
                  const std::string& content_type, const Headers& headers = {});

  /** @brief PUT JSON 请求 */
  HttpResult Put(const std::string& path, const std::string& json_body,
                 const Headers& headers = {});

  /** @brief DELETE 请求 */
  HttpResult Delete(const std::string& path, const Query& query = {}, const Headers& headers = {});

  /**
   * @brief 下载文件到本地
   * @return HttpResult，body 为空，status_code 表示结果
   */
  HttpResult Download(const std::string& path, const std::string& file_path);

  // ---------- 异步请求（回调在 cpr 线程池执行） ----------

  using Callback = std::function<void(HttpResult)>;

  void GetAsync(const std::string& path, const Query& query, const Headers& headers, Callback cb);
  void PostAsync(const std::string& path, const std::string& json_body, const Headers& headers,
                 Callback cb);

  // ---------- 工具 ----------

  // base_url + path 拼接（处理斜杠），query 参数拼成 "a=1&b=2"（已 URL 编码）
  static std::string BuildUrl(const std::string& base_url, const std::string& path);

 private:
  // 实际执行：method 为 "GET"/"POST"/"PUT"/"DELETE"
  HttpResult DoRequest(const std::string& method, const std::string& full_url,
                       const std::string& body, const std::string& content_type,
                       const Query& query, const Headers& headers);

  // 按 cfg_.max_retries 重试：网络失败或 5xx 时重试，间隔指数递增
  HttpResult DoRequestWithRetry(const std::string& method, const std::string& full_url,
                                const std::string& body, const std::string& content_type,
                                const Query& query, const Headers& headers);

  // 合并公共 header 与请求 header（后者覆盖前者）
  Headers MergeHeaders(const Headers& extra) const;

  std::string base_url_;
  RequestConfig cfg_;
  Headers default_headers_;  // 公共 header（配置了会话级 token/UA 时使用）
};

}  // namespace http
