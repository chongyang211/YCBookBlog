// Copyright © 1998 - 2023 Tencent. All Rights Reserved.

#include "network/http_request.h"

#include "cpr/async.h"
#include "cross/comm/static_init.h"
#include "device/device.h"

namespace palm {

// #define BETA_993_USE_PROXY 0
#define BETA_993_USE_RESOLVE 1

#if BETA_993_USE_PROXY
static constexpr char k993Proxy[] = "http://109.244.184.122:8080";
static constexpr char k993ProxyUser[] = "developer";
static constexpr char k993ProxyPwd[] = "Paytest@123";
#elif BETA_993_USE_RESOLVE
static constexpr char kPayDomain[] = "wxpay.iot.wechatpay.cn";
static constexpr char kPayComDomain[] = "wxpay.iot-tencent.com";
static constexpr char kPayLegacyDomain[] = "payapp.weixin.qq.com";  // already blocked by TELCOMs
static constexpr char k993ResolveIp[] = "81.71.56.251";
#endif

static constexpr char k993RouteHeader[] = "Wepaytest-Proxyip";

static struct curl_slist *k993CurlResv = nullptr;

void HttpRequest::CurlShareLockFunc(CURL *, curl_lock_data data, curl_lock_access, void *userptr) {
  curl_mutexes_[data].lock();
}
void HttpRequest::CurlShareUnlockFunc(CURL *, curl_lock_data data, curl_lock_access, void *userptr) {
  curl_mutexes_[data].unlock();
}

HttpRequest::HttpRequest() {
  static cross::comm::StaticInitializer sh_init([]() {
    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl_sh_ = curl_share_init();
    curl_share_setopt(curl_sh_, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);
    curl_share_setopt(curl_sh_, CURLSHOPT_SHARE, CURL_LOCK_DATA_SSL_SESSION);
    curl_share_setopt(curl_sh_, CURLSHOPT_SHARE, CURL_LOCK_DATA_CONNECT);

    curl_share_setopt(curl_sh_, CURLSHOPT_LOCKFUNC, CurlShareLockFunc);
    curl_share_setopt(curl_sh_, CURLSHOPT_UNLOCKFUNC, CurlShareUnlockFunc);

#if BETA_993_USE_RESOLVE
    if (Device::UseBeta()) {  // init only once
      k993CurlResv = curl_slist_append(k993CurlResv, (std::string(kPayDomain) + ":443:" + k993ResolveIp).c_str());
      k993CurlResv = curl_slist_append(k993CurlResv, (std::string(kPayComDomain) + ":443:" + k993ResolveIp).c_str());
      k993CurlResv = curl_slist_append(k993CurlResv, (std::string(kPayLegacyDomain) + ":443:" + k993ResolveIp).c_str());
    }
#endif
    // increase CPR threadpool max thread num, MUST run first !
    cpr::async::startup(CPR_DEFAULT_THREAD_POOL_MIN_THREAD_NUM, std::max(CPR_DEFAULT_THREAD_POOL_MAX_THREAD_NUM, 32u));
  });

  ss_ = std::make_shared<cpr::Session>();
  curl_easy_setopt(ss_->GetCurlHolder()->handle, CURLOPT_SHARE, curl_sh_);
  // set 993 proxy
  if (Device::UseBeta()) {
#if BETA_993_USE_PROXY
    ss_->SetProxies(HttpRequest::Proxies{{"http", k993Proxy}, {"https", k993Proxy}});
    ss_->SetProxyAuth(
        HttpRequest::ProxyAuthentication{{"http", HttpRequest::EncodedAuthentication{k993ProxyUser, k993ProxyPwd}},
                                         {"https", HttpRequest::EncodedAuthentication{k993ProxyUser, k993ProxyPwd}}});
#elif BETA_993_USE_RESOLVE
    curl_easy_setopt(ss_->GetCurlHolder()->handle, CURLOPT_RESOLVE, k993CurlResv);
#endif
    ss_->SetHeader(HttpRequest::Header{{k993RouteHeader, Device::GetBetaEnvRoute()}});
  }
}

void HttpRequest::SendHttpRequestImpl(asio::any_completion_handler<void(cpr::Response)> handler,
                                      asio::any_io_executor ex, std::shared_ptr<cpr::Session> sess,
                                      HttpMethodFunc func) {
  cpr::async([handler = std::move(handler), ex = std::move(ex), sess, func]() mutable {
    auto url = sess->GetFullRequestUrl();
    LOG_I("cpr requesting, url: {}", url);
    auto r = (sess.get()->*func)();
    LOG_I("cpr request done, url: {}", url);
    asio::post(ex, [handler = std::move(handler), r = std::move(r)]() mutable { std::move(handler)(r); });
  });
}

HttpRequest::Response HttpRequest::SendHttpRequestSyncImpl(std::shared_ptr<cpr::Session> sess, HttpMethodFunc func) {
  auto url = sess->GetFullRequestUrl();
  LOG_I("cpr requesting, url: {}", url);
  auto r = (sess.get()->*func)();
  LOG_I("cpr request done, url: {}", url);
  return r;
}

void *HttpRequest::curl_sh_ = nullptr;
std::array<std::mutex, CURL_LOCK_DATA_LAST> HttpRequest::curl_mutexes_ = {};
}  // namespace palm
