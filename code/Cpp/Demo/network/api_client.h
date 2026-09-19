// Copyright © 1998 - 2026 Tencent. All Rights Reserved.

// 通用 API 调用层 v2：在 HttpClient（拦截器链）之上做 JSON 化，
// 提供 Retrofit 风格的 Response<T>：传 URL + JSON，拿回业务结构体。

#pragma once

#include <string>

#include "network/http_client.h"
#include "nlohmann/json.hpp"

namespace api {

using http::Headers;
using http::Query;
using http::Response;

/**
 * @brief JSON API 结果（响应体自动解析为 nlohmann::json）
 *
 * 失败三分类（Ok() 为 false 时）：
 *   status_code == 0     网络层失败，error 有值
 *   status_code >= 400   HTTP 层失败
 *   parse_failed == true HTTP 200 但响应体不是合法 JSON
 */
struct ApiResult {
  int32_t status_code = 0;
  nlohmann::json data;      // 成功时为解析后的 JSON
  Headers headers;          // 响应头（便于读取 trace-id / 限流头等）
  std::string raw_body;
  std::string error;
  bool parse_failed = false;

  bool Ok() const {
    return error.empty() && !parse_failed && status_code >= 200 && status_code < 300;
  }
};

/**
 * @brief 通用 API 客户端
 *
 * 用法：
 *   api::ApiClient api("https://api.example.com");
 *   auto r = api.GetJson("/v1/user", {{"id","42"}});
 *   if (r.Ok()) { auto name = r.data["name"]; }
 *
 *   // 业务结构体直出（T 需提供 static T FromJson(const json&)）
 *   auto u = api.Get<User>("/v1/user/42");
 *   if (u.IsSuccess()) { u.Data().name; }
 */
class ApiClient {
 public:
  explicit ApiClient(std::string base_url, http::ClientConfig cfg = {});

  // ---------- 配置透传（含拦截器） ----------
  void SetBaseUrl(const std::string& base_url);
  void SetDefaultHeader(const std::string& key, const std::string& value);
  void SetBearerToken(const std::string& token);
  void AddInterceptor(http::InterceptorPtr interceptor);  // 自定义拦截器（签名/埋点…）
  http::ClientConfig& config();
  http::HttpClient& raw() { return http_; }  // 需要原始能力（下载/multipart）时使用

  // ---------- JSON 接口 ----------
  ApiResult GetJson(const std::string& path, const Query& query = {}, const Headers& headers = {});
  ApiResult PostJson(const std::string& path, const nlohmann::json& body,
                     const Headers& headers = {});
  ApiResult PutJson(const std::string& path, const nlohmann::json& body,
                    const Headers& headers = {});
  ApiResult DeleteJson(const std::string& path, const Query& query = {},
                       const Headers& headers = {});
  ApiResult PostRaw(const std::string& path, const std::string& body,
                    const std::string& content_type, const Headers& headers = {});

  // ---------- 泛型接口（Retrofit 风格 Response<T>） ----------

  /** @brief GET + 反序列化 */
  template <typename T>
  http::ResponseT<T> Get(const std::string& path, const Query& query = {},
                         const Headers& headers = {}) {
    return ToTyped<T>(GetJson(path, query, headers));
  }

  /** @brief POST + 反序列化（ReqT 需提供 ToJson() const） */
  template <typename RespT, typename ReqT>
  http::ResponseT<RespT> Post(const std::string& path, const ReqT& req,
                              const Headers& headers = {}) {
    return ToTyped<RespT>(PostJson(path, req.ToJson(), headers));
  }

  /** @brief PUT + 反序列化 */
  template <typename RespT, typename ReqT>
  http::ResponseT<RespT> Put(const std::string& path, const ReqT& req,
                             const Headers& headers = {}) {
    return ToTyped<RespT>(PutJson(path, req.ToJson(), headers));
  }

  // ---------- 文件 ----------
  http::Response Download(const std::string& path, const std::string& file_path);
  ApiResult Upload(const std::string& path, const std::string& field_name,
                   const std::string& file_path, const Headers& headers = {});

 private:
  static ApiResult ToApiResult(const http::Response& r);

  template <typename T>
  static http::ResponseT<T> ToTyped(const ApiResult& r) {
    http::ResponseT<T> ret;
    ret.status_code = r.status_code;
    ret.raw_body = r.raw_body;
    ret.error = r.error;
    ret.parse_failed = r.parse_failed;
    if (r.Ok()) {
      try {
        ret.data = T::FromJson(r.data);
      } catch (const std::exception& ex) {
        ret.parse_failed = true;
        ret.error = std::string("FromJson failed: ") + ex.what();
      }
    }
    return ret;
  }

  http::HttpClient http_;
};

}  // namespace api
