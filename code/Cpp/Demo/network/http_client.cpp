// Copyright © 1998 - 2026 Tencent. All Rights Reserved.

#include "network/http_client.h"

#include <fstream>
#include <thread>

#include "cpr/cpr.h"

namespace http {

namespace {

// URL 编码查询参数
std::string BuildQueryString(const Query& query) {
  if (query.empty()) return "";
  std::string qs;
  for (const auto& [k, v] : query) {
    if (!qs.empty()) qs += "&";
    qs += cpr::util::urlEncode(k) + "=" + cpr::util::urlEncode(v);
  }
  return qs;
}

// 网络层错误转可读字符串
std::string CprErrorToString(const cpr::Error& e) {
  if (e) return e.message.empty() ? "curl error code: " + std::to_string(static_cast<int>(e.code))
                                  : e.message;
  return "";
}

// cpr::Response → 统一 HttpResult
HttpResult ToHttpResult(const cpr::Response& r) {
  HttpResult ret;
  ret.status_code = r.status_code;
  ret.body = r.text;
  ret.error = CprErrorToString(r.error);
  ret.final_url = r.url.str();
  ret.elapsed_ms = static_cast<long>(r.elapsed);
  return ret;
}

}  // namespace

HttpClient::HttpClient(std::string base_url, RequestConfig cfg)
    : base_url_(std::move(base_url)), cfg_(cfg) {}

void HttpClient::SetBaseUrl(const std::string& base_url) { base_url_ = base_url; }

void HttpClient::SetDefaultHeader(const std::string& key, const std::string& value) {
  default_headers_[key] = value;
}

void HttpClient::RemoveDefaultHeader(const std::string& key) { default_headers_.erase(key); }

std::string HttpClient::BuildUrl(const std::string& base_url, const std::string& path) {
  if (path.empty()) return base_url;
  if (path.rfind("http://", 0) == 0 || path.rfind("https://", 0) == 0) return path;  // 完整 URL 直接用
  std::string base = base_url;
  while (!base.empty() && base.back() == '/') base.pop_back();
  if (path.front() != '/') return base + "/" + path;
  return base + path;
}

Headers HttpClient::MergeHeaders(const Headers& extra) const {
  Headers merged = default_headers_;
  for (const auto& [k, v] : extra) merged[k] = v;  // 请求级覆盖公共级
  return merged;
}

HttpResult HttpClient::DoRequest(const std::string& method, const std::string& full_url,
                                 const std::string& body, const std::string& content_type,
                                 const Query& query, const Headers& headers) {
  // URL 与查询参数拼接：已有 '?' 用 '&' 续接，否则用 '?' 开新查询串
  std::string sep;
  if (!query.empty()) sep = (full_url.find('?') == std::string::npos) ? "?" : "&";
  cpr::Url url{full_url + sep + BuildQueryString(query)};

  // 公共 header 与请求级 header 合并（⚠️ begin/end 必须来自同一个容器，
  // 曾经写成两次调用 MergeHeaders 的临时对象配对迭代器，直接段错误）
  Headers merged = MergeHeaders(headers);
  cpr::Header hdr{merged.begin(), merged.end()};
  if (!content_type.empty()) hdr["Content-Type"] = content_type;

  cpr::Session session;
  session.SetUrl(url);
  session.SetHeader(hdr);
  session.SetTimeout(cfg_.timeout);
  session.SetConnectTimeout(cfg_.connect_timeout);
  session.SetOption(cpr::VerifySsl{cfg_.verify_ssl});
  session.SetRedirect(cpr::Redirect{cfg_.follow_redirects ? 3L : 0L});
  if (!body.empty()) session.SetBody(cpr::Body{body});

  cpr::Response r;
  if (method == "GET") {
    r = session.Get();
  } else if (method == "POST") {
    r = session.Post();
  } else if (method == "PUT") {
    r = session.Put();
  } else if (method == "DELETE") {
    r = session.Delete();
  } else {
    return HttpResult{0, "", "unsupported method: " + method, "", 0};
  }
  return ToHttpResult(r);
}

HttpResult HttpClient::DoRequestWithRetry(const std::string& method, const std::string& full_url,
                                           const std::string& body, const std::string& content_type,
                                           const Query& query, const Headers& headers) {
  HttpResult result;
  int attempts = cfg_.max_retries + 1;
  for (int i = 0; i < attempts; i++) {
    result = DoRequest(method, full_url, body, content_type, query, headers);

    // 成功（含 4xx 业务错）：4xx 是调用方的错，重试无意义，直接返回
    bool retryable = result.NetworkFailed() || result.status_code >= 500;
    if (!retryable || i == attempts - 1) return result;

    int wait = cfg_.retry_interval_ms * (1 << i);  // 200ms, 400ms, 800ms...
    std::this_thread::sleep_for(std::chrono::milliseconds(wait));
  }
  return result;
}

HttpResult HttpClient::Get(const std::string& path, const Query& query, const Headers& headers) {
  return DoRequestWithRetry("GET", BuildUrl(base_url_, path), "", "", query, headers);
}

HttpResult HttpClient::Post(const std::string& path, const std::string& json_body,
                            const Headers& headers) {
  return DoRequestWithRetry("POST", BuildUrl(base_url_, path), json_body, "application/json", {},
                            headers);
}

HttpResult HttpClient::Post(const std::string& path, const std::string& body,
                            const std::string& content_type, const Headers& headers) {
  return DoRequestWithRetry("POST", BuildUrl(base_url_, path), body, content_type, {}, headers);
}

HttpResult HttpClient::Put(const std::string& path, const std::string& json_body,
                           const Headers& headers) {
  return DoRequestWithRetry("PUT", BuildUrl(base_url_, path), json_body, "application/json", {},
                            headers);
}

HttpResult HttpClient::Delete(const std::string& path, const Query& query,
                              const Headers& headers) {
  return DoRequestWithRetry("DELETE", BuildUrl(base_url_, path), "", "", query, headers);
}

HttpResult HttpClient::Download(const std::string& path, const std::string& file_path) {
  std::ofstream fp(file_path, std::ios::binary);
  if (!fp.is_open()) {
    return HttpResult{0, "", "open file failed: " + file_path, "", 0};
  }
  cpr::Session session;
  session.SetUrl(cpr::Url{BuildUrl(base_url_, path)});
  session.SetTimeout(cfg_.timeout);
  session.SetConnectTimeout(cfg_.connect_timeout);
  session.SetOption(cpr::VerifySsl{cfg_.verify_ssl});

  HttpResult ret;
  cpr::Response r = session.Download(fp);
  fp.close();

  ret.status_code = r.status_code;
  ret.error = CprErrorToString(r.error);
  ret.final_url = r.url.str();
  ret.elapsed_ms = static_cast<long>(r.elapsed);
  // Download 的 body 为空：成功时 body 置为落盘文件路径，方便调用方使用
  ret.body = ret.Ok() ? file_path : "";
  return ret;
}

void HttpClient::GetAsync(const std::string& path, const Query& query, const Headers& headers,
                          Callback cb) {
  // 拷贝所需状态到闭包，避免悬垂引用
  auto task = [this, path, query, headers, cb]() {
    // 注意：调用方需保证 HttpClient 生命周期覆盖回调执行期
    cb(Get(path, query, headers));
  };
  cpr::async(std::move(task));
}

void HttpClient::PostAsync(const std::string& path, const std::string& json_body,
                           const Headers& headers, Callback cb) {
  auto task = [this, path, json_body, headers, cb]() {
    cb(Post(path, json_body, headers));
  };
  cpr::async(std::move(task));
}

}  // namespace http
