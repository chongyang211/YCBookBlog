// 通用网络库 —— JSON API 层实现

#include "network/api_client.h"

namespace api {

ApiClient::ApiClient(std::string base_url, http::ClientConfig cfg)
    : http_(std::move(base_url), cfg) {}

void ApiClient::SetBaseUrl(const std::string& base_url) { http_.SetBaseUrl(base_url); }

void ApiClient::SetDefaultHeader(const std::string& key, const std::string& value) {
  http_.SetDefaultHeader(key, value);
}

void ApiClient::SetBearerToken(const std::string& token) {
  http_.SetDefaultHeader("Authorization", "Bearer " + token);
}

void ApiClient::AddInterceptor(http::InterceptorPtr interceptor) {
  http_.AddInterceptor(std::move(interceptor));
}

http::ClientConfig& ApiClient::config() { return http_.config(); }

ApiResult ApiClient::ToApiResult(const http::Response& r) {
  ApiResult ret;
  ret.status_code = r.status_code;
  ret.raw_body = r.body;
  ret.headers = r.headers;

  if (r.NetworkFailed()) {
    ret.error = r.error;
    return ret;
  }
  if (r.status_code >= 400) {
    ret.error = "HTTP " + std::to_string(r.status_code);
    return ret;
  }
  if (r.body.empty()) return ret;  // 204 No Content
  try {
    ret.data = nlohmann::json::parse(r.body);
  } catch (const std::exception& ex) {
    ret.parse_failed = true;
    ret.error = std::string("JSON parse failed: ") + ex.what();
  }
  return ret;
}

ApiResult ApiClient::GetJson(const std::string& path, const Query& query, const Headers& headers) {
  return ToApiResult(http_.Get(path, query, headers));
}

ApiResult ApiClient::PostJson(const std::string& path, const nlohmann::json& body,
                              const Headers& headers) {
  return ToApiResult(http_.Post(path, body.dump(), headers));
}

ApiResult ApiClient::PutJson(const std::string& path, const nlohmann::json& body,
                             const Headers& headers) {
  return ToApiResult(http_.Put(path, body.dump(), headers));
}

ApiResult ApiClient::DeleteJson(const std::string& path, const Query& query,
                                const Headers& headers) {
  return ToApiResult(http_.Delete(path, query, headers));
}

ApiResult ApiClient::PostRaw(const std::string& path, const std::string& body,
                             const std::string& content_type, const Headers& headers) {
  return ToApiResult(http_.Post(path, body, content_type, headers));
}

http::Response ApiClient::Download(const std::string& path, const std::string& file_path) {
  return http_.Download(path, file_path);
}

ApiResult ApiClient::Upload(const std::string& path, const std::string& field_name,
                            const std::string& file_path, const Headers& headers) {
  http::Response r = http_.Post(path)
                         .Multipart({{field_name, file_path}})
                         .Headers(headers)
                         .Send();
  return ToApiResult(r);
}

}  // namespace api
