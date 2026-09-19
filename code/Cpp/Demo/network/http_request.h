// Copyright © 1998 - 2023 Tencent. All Rights Reserved.

#pragma once

#include <memory>
#include <mutex>
#include <utility>

#include "boost/asio/any_completion_handler.hpp"
#include "cpr/async.h"
#include "cpr/session.h"

#include "comm/comm.h"

namespace palm {
class HttpRequest {
 public:
  using Url = cpr::Url;
  using Parameters = cpr::Parameters;
  using Header = cpr::Header;
  using Payload = cpr::Payload;
  using Body = cpr::Body;
  using Multipart = cpr::Multipart;
  using Buffer = cpr::Buffer;
  using File = cpr::File;
  using Response = cpr::Response;
  using Proxies = cpr::Proxies;
  using ProxyAuthentication = cpr::ProxyAuthentication;
  using EncodedAuthentication = cpr::EncodedAuthentication;
  using Error = cpr::Error;
  using ErrorCode = cpr::ErrorCode;
  using LimitRate = cpr::LimitRate;
  using WriteCallback = cpr::WriteCallback;
  using ProgressCallback = cpr::ProgressCallback;

  // common methods forward
  inline void SetUrl(const Url& url) const { ss_->SetUrl(url); }
  inline void SetParameters(const Parameters& parameters) const { ss_->SetParameters(parameters); }
  inline void SetParameters(Parameters&& parameters) const { ss_->SetParameters(std::move(parameters)); }
  inline void SetHeader(const Header& header) const { ss_->SetHeader(header); }
  inline void UpdateHeader(const Header& header) const { ss_->UpdateHeader(header); }
  inline void SetTimeout(const std::chrono::milliseconds& timeout) const { ss_->SetTimeout(timeout); }
  inline void SetConnectTimeout(const std::chrono::milliseconds& timeout) const { ss_->SetConnectTimeout(timeout); }
  inline void SetPayload(Payload&& payload) const { ss_->SetPayload(std::move(payload)); }
  inline void SetPayload(const Payload& payload) const { ss_->SetPayload(payload); }
  inline void SetProxies(Proxies&& proxies) const { ss_->SetProxies(std::move(proxies)); }
  inline void SetProxies(const Proxies& proxies) const { ss_->SetProxies(proxies); }
  inline void SetProxyAuth(ProxyAuthentication&& proxy_auth) const { ss_->SetProxyAuth(std::move(proxy_auth)); }
  inline void SetProxyAuth(const ProxyAuthentication& proxy_auth) const { ss_->SetProxyAuth(proxy_auth); }
  inline void SetMultipart(Multipart&& multipart) const { ss_->SetMultipart(std::move(multipart)); }
  inline void SetMultipart(const Multipart& multipart) const { ss_->SetMultipart(multipart); }
  inline void SetBody(Body&& body) const { ss_->SetBody(std::move(body)); }
  inline void SetBody(const Body& body) const { ss_->SetBody(body); }
  inline void SetLimitRate(const LimitRate& limit_rate) const { ss_->SetLimitRate(limit_rate); }
  inline void SetWriteCallback(const WriteCallback& write_callback) const { ss_->SetWriteCallback(write_callback); }
  inline void SetProgressCallback(const ProgressCallback& callback) const { ss_->SetProgressCallback(callback); }

  HttpRequest();
  // 短连接主要接口。
  [[nodiscard]] inline auto PostSync() const { return SendHttpRequestSyncImpl(ss_, &cpr::Session::Post); }
  // 腾讯地图主要使用。
  [[nodiscard]] inline auto GetSync() const { return SendHttpRequestSyncImpl(ss_, &cpr::Session::Get); }
  // 主要日志上传的请求。RESTFUL 格式。
  [[nodiscard]] inline auto PutSync() const { return SendHttpRequestSyncImpl(ss_, &cpr::Session::Put); }
  // need cpr::session::get->DowmLoad; 工具用于下载资源
  inline Response DownLoadSync(std::ofstream& file) const { return ss_->Download(file); }

 private:
  using HttpMethodFunc = cpr::Response (cpr::Session::*)();
  static void SendHttpRequestImpl(asio::any_completion_handler<void(cpr::Response)> handler, asio::any_io_executor ex,
                                  std::shared_ptr<cpr::Session> sess, HttpMethodFunc func);
  static Response SendHttpRequestSyncImpl(std::shared_ptr<cpr::Session> sess, HttpMethodFunc func);
  static void CurlShareLockFunc(CURL*, curl_lock_data, curl_lock_access, void*);
  static void CurlShareUnlockFunc(CURL*, curl_lock_data, curl_lock_access, void*);

public:
    template <typename CompletionToken>
    inline auto GetAsync(asio::any_io_executor ex, CompletionToken&& token) {
      return asio::async_initiate<CompletionToken, void(Response)>(SendHttpRequestImpl, token, std::move(ex), ss_,
                                                                  &cpr::Session::Get);
    }

    template <typename CompletionToken>
    inline auto PostAsync(asio::any_io_executor ex, CompletionToken&& token) {
      return asio::async_initiate<CompletionToken, void(Response)>(SendHttpRequestImpl, token, std::move(ex), ss_,
                                                                  &cpr::Session::Post);
    }

    template <typename CompletionToken>
    inline auto PutAsync(asio::any_io_executor ex, CompletionToken&& token) {
      return asio::async_initiate<CompletionToken, void(Response)>(SendHttpRequestImpl, token, std::move(ex), ss_,
                                                                  &cpr::Session::Put);
    }

  std::shared_ptr<cpr::Session> ss_ = nullptr;
  static void* curl_sh_;
  static std::array<std::mutex, CURL_LOCK_DATA_LAST> curl_mutexes_;
};

}  // namespace palm
