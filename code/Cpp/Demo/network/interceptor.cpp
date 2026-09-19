// Copyright © 1998 - 2026 Tencent. All Rights Reserved.

#include "network/interceptor.h"

#include <sstream>
#include <thread>

namespace http {

const char* MethodToString(Method m) {
  switch (m) {
    case Method::kGet:    return "GET";
    case Method::kPost:   return "POST";
    case Method::kPut:    return "PUT";
    case Method::kDelete: return "DELETE";
    case Method::kPatch:  return "PATCH";
  }
  return "UNKNOWN";
}

Response Chain::Proceed(Request request) {
  if (index_ >= interceptors_.size()) {
    return executor_ ? executor_(request) : Response{};
  }
  Chain next(request, interceptors_, index_ + 1, executor_);
  return interceptors_[index_]->Intercept(next);
}

namespace interceptor {

namespace {

// 仅用标准库拼接日志（避免引入 fmt 等额外依赖）
std::string Join(const std::string& method, const std::string& url, int32_t status,
                 long elapsed_ms, size_t body_size, const std::string& error) {
  std::ostringstream oss;
  oss << method << " " << url;
  if (status > 0) oss << " status=" << status;
  oss << " elapsed=" << elapsed_ms << "ms size=" << body_size;
  if (!error.empty()) oss << " error=" << error;
  return oss.str();
}

}  // namespace

Response LoggingInterceptor::Intercept(Chain& chain) {
  const Request& req = chain.request();
  if (logger_) logger_("[http] -> " + std::string(MethodToString(req.method)) + " " + req.url);

  auto start = std::chrono::steady_clock::now();
  Response resp = chain.Proceed();
  long elapsed = static_cast<long>(
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start)
          .count());

  if (logger_) {
    logger_("[http] <- " + Join(MethodToString(req.method), req.url, resp.status_code, elapsed,
                                resp.body.size(), resp.error));
  }
  return resp;
}

Response HeaderInterceptor::Intercept(Chain& chain) {
  Request req = chain.request();              // 拷一份，避免污染上游调用方持有的 Request
  for (const auto& [k, v] : headers_) req.headers[k] = v;
  return chain.Proceed(req);
}

Response BearerAuthInterceptor::Intercept(Chain& chain) {
  Request req = chain.request();
  req.headers["Authorization"] = "Bearer " + token_;
  return chain.Proceed(req);
}

Response RetryInterceptor::Intercept(Chain& chain) {
  Response resp;
  for (int attempt = 1; attempt <= policy_.max_attempts; attempt++) {
    resp = chain.Proceed();
    bool should_retry = resp.NetworkFailed() || (policy_.retry_on_5xx && resp.status_code >= 500);
    if (!should_retry || attempt == policy_.max_attempts) return resp;

    long delay = policy_.base_delay.count() * (1 << (attempt - 1));  // 200/400/800...
    if (delay > policy_.max_delay_ms) delay = policy_.max_delay_ms;
    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
  }
  return resp;
}

Response SigningInterceptor::Intercept(Chain& chain) {
  Request req = chain.request();
  // 用原始 path（而非完整 URL）参与签名，与服务端约定保持一致
  req.headers[header_] = signer_(MethodToString(req.method), req.path, req.body);
  return chain.Proceed(req);
}

}  // namespace interceptor
}  // namespace http
