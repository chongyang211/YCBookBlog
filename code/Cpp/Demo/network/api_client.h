// Copyright © 1998 - 2026 Tencent. All Rights Reserved.

// 通用 API 调用层：在 HttpClient 之上做 JSON 化，
// 对齐原 palm 项目 RequestEngine 的"三段式"（序列化→请求→解析），
// 但把 40 行样板收敛为一个泛型调用。

#pragma once

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "network/http_client.h"
#include "nlohmann/json.hpp"

namespace api {

using http::Headers;
using http::Query;

/**
 * @brief 统一 API 响应结构（JSON 化）
 *
 * 失败分类（Ok() 为 false 时）：
 *   status_code == 0        网络层失败（连不上/超时），error 有值
 *   status_code >= 400      HTTP 层失败（4xx/5xx），error 为状态码描述
 *   parse_failed == true    HTTP 200 但响应体不是合法 JSON
 */
struct ApiResult {
  int32_t status_code = 0;      // 0 = 网络失败；否则为 HTTP 状态码
  nlohmann::json data;          // 成功时为解析后的 JSON；失败为 null
  std::string raw_body;         // 原始响应体（解析失败时排查用）
  std::string error;            // 失败原因
  bool parse_failed = false;    // HTTP 成功但 JSON 解析失败

  bool Ok() const { return error.empty() && !parse_failed && status_code >= 200 && status_code < 300; }
};

/**
 * @brief 通用 API 客户端：传 URL + JSON，拿回 JSON
 *
 * 用法示例：
 *   api::ApiClient client("https://api.example.com");
 *   auto r = client.GetJson("/v1/user", {{"id", "42"}});
 *   if (r.Ok()) { std::string name = r.data["name"]; }
 */
class ApiClient {
 public:
  explicit ApiClient(std::string base_url, http::RequestConfig cfg = {});

  // 透传给 HttpClient 的配置能力
  void SetBaseUrl(const std::string& base_url);
  void SetDefaultHeader(const std::string& key, const std::string& value);  // 如 Authorization: Bearer xx
  void SetBearerToken(const std::string& token);  // 快捷设置 Bearer Token
  http::RequestConfig& config();

  // ---------- JSON API ----------

  /** @brief GET，响应体自动解析为 JSON */
  ApiResult GetJson(const std::string& path, const Query& query = {}, const Headers& headers = {});

  /** @brief POST，body 为任意可转 JSON 的值（对象/数组/字面量均可） */
  ApiResult PostJson(const std::string& path, const nlohmann::json& body,
                     const Headers& headers = {});

  /** @brief PUT，body 为 JSON */
  ApiResult PutJson(const std::string& path, const nlohmann::json& body,
                    const Headers& headers = {});

  /** @brief DELETE，响应体自动解析为 JSON */
  ApiResult DeleteJson(const std::string& path, const Query& query = {},
                       const Headers& headers = {});

  // ---------- 泛型便捷调用（自动反序列化到业务结构体） ----------

  /**
   * @brief GET + 反序列化到 RespT
   * @tparam RespT 业务响应类型，需提供 static RespT FromJson(const nlohmann::json&)
   *               （与原 palm::entity 的约定完全一致，现有 entity 可直接复用）
   */
  template <typename RespT>
  bool Get(const std::string& path, RespT& out, const Query& query = {},
           const Headers& headers = {}) {
    auto r = GetJson(path, query, headers);
    return Unpack(r, out);
  }

  /**
   * @brief POST + 反序列化到 RespT
   * @tparam ReqT  请求类型，需提供 ToJson() const
   * @tparam RespT 响应类型，需提供 static FromJson(const nlohmann::json&)
   */
  template <typename ReqT, typename RespT>
  bool Post(const std::string& path, const ReqT& req, RespT& out, const Headers& headers = {}) {
    auto r = PostJson(path, req.ToJson(), headers);
    return Unpack(r, out);
  }

  // ---------- 非 JSON（表单/protobuf 等） ----------

  ApiResult PostRaw(const std::string& path, const std::string& body,
                    const std::string& content_type, const Headers& headers = {});

  /** @brief 文件下载 */
  http::HttpResult Download(const std::string& path, const std::string& file_path);

 private:
  // 统一把 HttpResult 转 ApiResult（含 JSON 解析与错误归一）
  static ApiResult ToApiResult(const http::HttpResult& r);

  // ApiResult → 业务结构体（Ok 时调 FromJson 反序列化）
  template <typename RespT>
  static bool Unpack(const ApiResult& r, RespT& out) {
    if (!r.Ok()) return false;
    try {
      out = RespT::FromJson(r.data);
      return true;
    } catch (const std::exception&) {
      return false;
    }
  }

  http::HttpClient http_;
};

}  // namespace api
