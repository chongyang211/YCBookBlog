// 通用网络库 —— 内置拦截器：日志 / 公共 Header / 重试 / Bearer 鉴权 / 签名。
//
// 拦截器存在的意义：把"每个请求都要做、但与业务无关"的横切逻辑
// （日志、鉴权、签名、重试、埋点、mock）从调用点剥离，集中在一处，
// 业务代码只管 path + body + 结果。

#pragma once

#include <string>
#include <vector>

#include "network/http_defs.h"

namespace http {
namespace interceptor {

/**
 * @brief 日志拦截器：打印请求与响应摘要（方法/URL/状态码/耗时）
 */
class LoggingInterceptor : public Interceptor {
 public:
  explicit LoggingInterceptor(Logger logger) : logger_(std::move(logger)) {}

  Response Intercept(Chain& chain) override;

 private:
  Logger logger_;
};

/**
 * @brief 公共 Header 拦截器：为所有请求注入 header（UA/AppId/trace 等）
 */
class HeaderInterceptor : public Interceptor {
 public:
  explicit HeaderInterceptor(Headers headers) : headers_(std::move(headers)) {}
  // 动态更新（如登录后写入 token）
  void Set(const std::string& key, const std::string& value) { headers_[key] = value; }

  Response Intercept(Chain& chain) override;

 private:
  Headers headers_;
};

/**
 * @brief Bearer 鉴权拦截器：等价于 HeaderInterceptor 的 Authorization 特化版
 */
class BearerAuthInterceptor : public Interceptor {
 public:
  explicit BearerAuthInterceptor(std::string token) : token_(std::move(token)) {}
  void SetToken(std::string token) { token_ = std::move(token); }

  Response Intercept(Chain& chain) override;

 private:
  std::string token_;
};

/**
 * @brief 重试拦截器：网络失败或 5xx 时按指数退避重试（4xx 不重试）
 *
 * 放在链的前面（越靠前，重试覆盖的下游拦截器越多）。
 */
class RetryInterceptor : public Interceptor {
 public:
  explicit RetryInterceptor(RetryPolicy policy) : policy_(policy) {}
  void set_policy(RetryPolicy policy) { policy_ = policy; }

  Response Intercept(Chain& chain) override;

 private:
  RetryPolicy policy_;
};

/**
 * @brief 签名拦截器：把"签名"从业务代码里剥离出来
 *
 * 典型场景：接口要求按 (method, path, body) 计算摘要并写入 Authorization/签名头，
 *            且不同接口可能用不同签名算法（多套签名 = 多个 Signer，互不影响）。
 * signer 签名为 (method, path, body) -> 签名值，拦截器负责写入指定 header。
 */
class SigningInterceptor : public Interceptor {
 public:
  using Signer = std::function<std::string(const std::string& method, const std::string& path,
                                           const std::string& body)>;
  explicit SigningInterceptor(Signer signer, std::string header = "Authorization")
      : signer_(std::move(signer)), header_(std::move(header)) {}

  Response Intercept(Chain& chain) override;

 private:
  Signer signer_;
  std::string header_;
};

}  // namespace interceptor
}  // namespace http
