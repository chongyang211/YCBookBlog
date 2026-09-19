// Copyright © 1998 - 2026 Tencent. All Rights Reserved.

#include "network/http_client.h"

#include <array>
#include <fstream>
#include <mutex>
#include <utility>

#include "cpr/cpr.h"
#include "network/interceptor.h"

namespace http {
namespace {

// curl share：跨 Session 复用 DNS 缓存 / SSL 会话 / TCP 连接，显著提升短连接性能。
// 参考原 palm::HttpRequest 的做法，但只保留必要的三种共享与线程安全锁。
class CurlShare {
 public:
  static CurlShare& Instance() {
    static CurlShare inst;
    return inst;
  }
  CURLSH* handle() { return share_; }

 private:
  CurlShare() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    share_ = curl_share_init();
    curl_share_setopt(share_, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);
    curl_share_setopt(share_, CURLSHOPT_SHARE, CURL_LOCK_DATA_SSL_SESSION);
    curl_share_setopt(share_, CURLSHOPT_SHARE, CURL_LOCK_DATA_CONNECT);
    curl_share_setopt(share_, CURLSHOPT_LOCKFUNC, &CurlShare::Lock);
    curl_share_setopt(share_, CURLSHOPT_UNLOCKFUNC, &CurlShare::Unlock);
  }
  ~CurlShare() {
    curl_share_cleanup(share_);
    curl_global_cleanup();
  }
  CurlShare(const CurlShare&) = delete;
  CurlShare& operator=(const CurlShare&) = delete;

  static void Lock(CURL*, curl_lock_data data, curl_lock_access, void*) { mutexes_[data].lock(); }
  static void Unlock(CURL*, curl_lock_data data, curl_lock_access, void*) {
    mutexes_[data].unlock();
  }

  CURLSH* share_ = nullptr;
  static std::array<std::mutex, CURL_LOCK_DATA_LAST> mutexes_;
};
std::array<std::mutex, CURL_LOCK_DATA_LAST> CurlShare::mutexes_{};

// cpr::Response → 统一 Response
Response ToResponse(const cpr::Response& r) {
  Response ret;
  ret.status_code = r.status_code;
  ret.body = r.text;
  ret.final_url = r.url.str();
  ret.elapsed_ms = static_cast<long>(r.elapsed);
  if (r.error) {
    ret.error = r.error.message.empty()
                    ? ("curl error code: " + std::to_string(static_cast<int>(r.error.code)))
                    : r.error.message;
  }
  for (const auto& [k, v] : r.header) ret.headers[k] = v;
  return ret;
}

// 用 cpr 真正发一次请求（拦截器链的终点）
Response SendWithCpr(const Request& req) {
  cpr::Session session;
  session.SetUrl(cpr::Url{req.url});
  session.SetTimeout(req.timeout);
  session.SetConnectTimeout(req.connect_timeout);
  session.SetOption(cpr::VerifySsl{req.verify_ssl});
  session.SetRedirect(cpr::Redirect{req.follow_redirects ? 3L : 0L});

  cpr::Header hdr{req.headers.begin(), req.headers.end()};
  if (!req.content_type.empty()) hdr["Content-Type"] = req.content_type;
  session.SetHeader(hdr);

  if (!req.multipart.empty()) {
    // cpr::Multipart 只有 initializer_list 构造，但 parts 是 public 成员：
    // 用首个 part 初始化，其余 emplace_back，从而支持任意数量的文件字段
    cpr::Multipart mp{{req.multipart.front().first, cpr::File{req.multipart.front().second}}};
    for (size_t i = 1; i < req.multipart.size(); i++) {
      mp.parts.emplace_back(req.multipart[i].first, cpr::File{req.multipart[i].second});
    }
    session.SetMultipart(std::move(mp));
  } else if (!req.body.empty()) {
    session.SetBody(cpr::Body{req.body});
  }

  // 复用 DNS/SSL/连接
  curl_easy_setopt(session.GetCurlHolder()->handle, CURLOPT_SHARE, CurlShare::Instance().handle());

  switch (req.method) {
    case Method::kGet:    return ToResponse(session.Get());
    case Method::kPost:   return ToResponse(session.Post());
    case Method::kPut:    return ToResponse(session.Put());
    case Method::kDelete: return ToResponse(session.Delete());
    case Method::kPatch:  return ToResponse(session.Patch());
  }
  return Response{};
}

}  // namespace

// ---------- HttpClient ----------

HttpClient::HttpClient(std::string base_url, ClientConfig cfg)
    : base_url_(std::move(base_url)), cfg_(cfg) {
  // 内置拦截器按推荐顺序装配：日志 → 重试（公共 header 在 Execute 中合并）
  if (cfg_.enable_logging && cfg_.logger) {
    interceptors_.push_back(std::make_shared<interceptor::LoggingInterceptor>(cfg_.logger));
  }
  if (cfg_.retry.max_attempts > 1) {
    interceptors_.push_back(std::make_shared<interceptor::RetryInterceptor>(cfg_.retry));
  }
}

std::string HttpClient::BuildUrl(const std::string& base_url, const std::string& path) {
  if (path.rfind("http://", 0) == 0 || path.rfind("https://", 0) == 0) return path;
  std::string base = base_url;
  while (!base.empty() && base.back() == '/') base.pop_back();
  std::string p = path.empty() ? "" : (path.front() == '/' ? path : "/" + path);
  return base + p;
}

std::string HttpClient::BuildQueryString(const Query& query) {
  std::string qs;
  for (const auto& [k, v] : query) {
    if (!qs.empty()) qs += "&";
    qs += cpr::util::urlEncode(k) + "=" + cpr::util::urlEncode(v);
  }
  return qs;
}

Response HttpClient::Execute(Request request) {
  // 补全默认配置
  if (request.timeout.count() == 0) request.timeout = cfg_.timeout;
  if (request.connect_timeout.count() == 0) request.connect_timeout = cfg_.connect_timeout;

  // 查询串拼接到 URL（拦截器可继续改写 request）
  if (!request.query.empty()) {
    std::string sep = (request.url.find('?') == std::string::npos) ? "?" : "&";
    request.url += sep + BuildQueryString(request.query);
  }
  // 合并 client 级 header（请求级优先）
  for (const auto& [k, v] : default_headers_) {
    if (request.headers.find(k) == request.headers.end()) request.headers[k] = v;
  }

  std::vector<InterceptorPtr> chain_copy = interceptors_;  // 快照，避免执行中被改
  Chain chain(request, chain_copy, 0, [](const Request& r) { return SendWithCpr(r); });
  return chain.Proceed();
}

void HttpClient::AddInterceptor(InterceptorPtr interceptor) {
  interceptors_.emplace_back(std::move(interceptor));
}

void HttpClient::ClearInterceptors() { interceptors_.clear(); }

void HttpClient::SetBaseUrl(const std::string& base_url) { base_url_ = base_url; }

void HttpClient::SetDefaultHeader(const std::string& key, const std::string& value) {
  default_headers_[key] = value;
}

void HttpClient::RemoveDefaultHeader(const std::string& key) { default_headers_.erase(key); }

// ---------- 链式入口 ----------

RequestBuilder HttpClient::Get(const std::string& path) {
  return RequestBuilder(this, Method::kGet, path);
}
RequestBuilder HttpClient::Post(const std::string& path) {
  return RequestBuilder(this, Method::kPost, path);
}
RequestBuilder HttpClient::Put(const std::string& path) {
  return RequestBuilder(this, Method::kPut, path);
}
RequestBuilder HttpClient::Delete(const std::string& path) {
  return RequestBuilder(this, Method::kDelete, path);
}
RequestBuilder HttpClient::Patch(const std::string& path) {
  return RequestBuilder(this, Method::kPatch, path);
}

// ---------- 便捷同步接口 ----------

Response HttpClient::Get(const std::string& path, const Query& query, const Headers& headers) {
  return this->Get(path).Query(query).Headers(headers).Send();
}

Response HttpClient::Post(const std::string& path, const std::string& json_body,
                          const Headers& headers) {
  return Post(path).Json(json_body).Headers(headers).Send();
}

Response HttpClient::Post(const std::string& path, const std::string& body,
                          const std::string& content_type, const Headers& headers) {
  return Post(path).Body(body, content_type).Headers(headers).Send();
}

Response HttpClient::Put(const std::string& path, const std::string& json_body,
                         const Headers& headers) {
  return Put(path).Json(json_body).Headers(headers).Send();
}

Response HttpClient::Delete(const std::string& path, const Query& query, const Headers& headers) {
  return Delete(path).Query(query).Headers(headers).Send();
}

Response HttpClient::Download(const std::string& path, const std::string& file_path) {
  std::ofstream fp(file_path, std::ios::binary);
  if (!fp.is_open()) return Response{0, "", {}, "", "open file failed: " + file_path, 0};

  Request req;
  req.method = Method::kGet;
  req.path = path;
  req.url = BuildUrl(base_url_, path);
  req.timeout = cfg_.timeout;
  req.connect_timeout = cfg_.connect_timeout;
  req.verify_ssl = cfg_.verify_ssl;
  for (const auto& [k, v] : default_headers_) req.headers[k] = v;

  cpr::Session session;
  session.SetUrl(cpr::Url{req.url});
  session.SetTimeout(req.timeout);
  session.SetOption(cpr::VerifySsl{req.verify_ssl});
  cpr::Header hdr{req.headers.begin(), req.headers.end()};
  session.SetHeader(hdr);
  curl_easy_setopt(session.GetCurlHolder()->handle, CURLOPT_SHARE, CurlShare::Instance().handle());

  Response ret = ToResponse(session.Download(fp));
  fp.close();
  if (ret.Ok()) ret.body = file_path;  // 成功后 body 置为落盘路径
  return ret;
}

void HttpClient::GetAsync(const std::string& path, const Query& query, const Headers& headers,
                          Callback cb) {
  auto task = [this, path, query, headers, cb]() { cb(Get(path, query, headers)); };
  cpr::async(std::move(task));
}

void HttpClient::PostAsync(const std::string& path, const std::string& json_body,
                           const Headers& headers, Callback cb) {
  auto task = [this, path, json_body, headers, cb]() { cb(Post(path, json_body, headers)); };
  cpr::async(std::move(task));
}

// ---------- RequestBuilder ----------

RequestBuilder::RequestBuilder(HttpClient* client, Method method, std::string path)
    : client_(client) {
  req_.method = method;
  req_.path = path;
  req_.url = HttpClient::BuildUrl(client_->base_url(), path);
  req_.timeout = client_->config().timeout;
  req_.connect_timeout = client_->config().connect_timeout;
  req_.verify_ssl = client_->config().verify_ssl;
  req_.follow_redirects = client_->config().follow_redirects;
}

// 注意：参数类型写全限定 http::Query / http::Headers ——
// 类内成员函数同名（Query/Headers）会遮蔽命名空间里的类型别名，
// 直接写 Query/Headers 会被解析成成员函数名而编译失败。
RequestBuilder& RequestBuilder::Query(http::Query q) {
  req_.query = std::move(q);
  return *this;
}
RequestBuilder& RequestBuilder::Header(const std::string& key, const std::string& value) {
  req_.headers[key] = value;
  return *this;
}
RequestBuilder& RequestBuilder::Headers(http::Headers h) {
  for (auto& [k, v] : h) req_.headers[k] = std::move(v);
  return *this;
}
RequestBuilder& RequestBuilder::Body(const std::string& body, const std::string& content_type) {
  req_.body = body;
  req_.content_type = content_type;
  return *this;
}
RequestBuilder& RequestBuilder::Json(const std::string& json_str) {
  return Body(json_str, "application/json");
}
RequestBuilder& RequestBuilder::Multipart(MultipartParts parts) {
  req_.multipart = std::move(parts);
  return *this;
}
RequestBuilder& RequestBuilder::Timeout(std::chrono::milliseconds t) {
  req_.timeout = t;
  return *this;
}
RequestBuilder& RequestBuilder::ConnectTimeout(std::chrono::milliseconds t) {
  req_.connect_timeout = t;
  return *this;
}
RequestBuilder& RequestBuilder::VerifySsl(bool on) {
  req_.verify_ssl = on;
  return *this;
}

Response RequestBuilder::Send() { return client_->Execute(req_); }

void RequestBuilder::SendAsync(std::function<void(Response)> cb) {
  Request snapshot = req_;  // 拷贝快照：builder 通常临时对象，异步执行时已析构
  HttpClient* client = client_;
  cpr::async([client, snapshot, cb]() mutable { cb(client->Execute(snapshot)); });
}

std::future<Response> RequestBuilder::SendAsync() {
  Request snapshot = req_;
  HttpClient* client = client_;
  auto task = std::make_shared<std::packaged_task<Response()>>(
      [client, snapshot]() mutable { return client->Execute(snapshot); });
  std::future<Response> fut = task->get_future();
  cpr::async([task]() { (*task)(); });
  return fut;
}

}  // namespace http
