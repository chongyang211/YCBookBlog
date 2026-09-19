// Copyright © 1998 - 2020 Tencent. All Rights Reserved.

#include "network/network.h"

#include <chrono>
#include <memory>
#include <sstream>

#include "boost/algorithm/hex.hpp"
#include "boost/asio/as_tuple.hpp"
#include "boost/asio/error.hpp"
#include "boost/asio/steady_timer.hpp"
#include "boost/asio/this_coro.hpp"
#include "boost/iostreams/copy.hpp"
#include "boost/iostreams/filter/gzip.hpp"
#include "boost/iostreams/filtering_streambuf.hpp"
#include "boost/system/errc.hpp"
#include "fmt/format.h"
#include "nlohmann/json.hpp"

#include "app/app.h"
#include "comm/constant/unified_errcode_face.h"
#include "comm/constant/unified_errcode_iot.h"
#include "cross/comm/base64.h"
#include "cross/comm/digest.h"
#include "cross/comm/logger.h"
#include "cross/comm/rand.h"
#include "cross/comm/time_util.h"
#include "cross/comm/timecounter.h"
#include "device/device.h"
#include "keystore/sekeystore.h"
#include "network/http_request.h"
#include "network/network_info.h"
#include "palm/palm_interface.h"
#include "network/network_stat.h"
#include "utils/string_util.h"
#include "activation/proto/PalmActivation.pb.h"

#include "sm.h"
namespace palm {

// use system bundled ssl ca for now
static constexpr char kDefaultSslPath[] = "/etc/ssl/certs";

// Payapp API Base URL
static constexpr char kPayAppBaseUrl[] = "https://wxpay.iot.wechatpay.cn/palmpayapp";

static bool use_shortlink_ = true;

// 固定签名密钥(用于 SIMPLE 签名方式)
static constexpr char kSimpleSignKey[] = "p59l3IqjYOTS044SDNSgBbJrGxvfLEqr";

// 检查IoT设备域名是否已设置的宏
#define CHECK_IOT_DEVICE_HOST_SET(error_code_var, error_msg_var)               \
  do {                                                                          \
    if (iot_device_host_.empty()) {                                            \
      LOG_E("iot_device_host_ is not set, call SetIotDeviceHost first");      \
      error_code_var = ERR_PALM_IOT_HOST_NOT_SET;                              \
      error_msg_var = "IoT device host not set";                               \
      return {error_code_var, ""};                                             \
    }                                                                           \
  } while (0)

Network::Network() = default;

Network::~Network() = default;

Network* Network::Instance() {
  static Network inst;
  return &inst;
}

void Network::SetIotDeviceHost(const std::string& domain) {
  iot_device_host_ = domain.find("https://") == 0 ? domain : ("https://" + domain);
  LOG_I("SetIotDeviceHost: host={}", iot_device_host_);
}

void Network::SetActivationSession(const std::string& domain,
                                   const std::string& session_id,
                                   const std::string& session_key) {
  SetIotDeviceHost(domain);
  session_id_ = session_id;
  session_key_ = session_key;
  LOG_I("SetActivationSession: domain={}, session_id={}", iot_device_host_, session_id_);
}

void Network::ClearActivationSession() {
  session_id_.clear();
  session_key_.clear();
  LOG_I("ClearActivationSession: session cleared");
}

struct InnerRequestRet {
  int32_t error = 0;
  std::string resp_data;
  std::string errmsg;

  InnerRequestRet() = default;

  InnerRequestRet(int32_t _error, std::string _resp_data, std::string _errmsg)
      : error(_error), resp_data(std::move(_resp_data)), errmsg(std::move(_errmsg)) {}
};
// 请替换为正确的
static constexpr char kIotmSignKeyId[] = "xxxxx";
// 请替换为正确的
static constexpr char kIotmSignSecret[] =
    "\x11\x11\x11\x11\x11\x11\x11\x11"
    "\x11\x11\x11\x11\x11\x11\x11\x11";

static std::string DecodeMixedSecret(const std::string& mixed_secret, bool b64_format = true) {
  // 请替换为正确的
  static const char* raw_mask =
      "\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11"
      "\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11";
  int raw_mask_len = static_cast<int>(strlen(raw_mask));  // assume rawMask not contain '\0' in middle

  std::string raw(mixed_secret);
  for (int i = 0, j = 0; i < raw.length(); i++, j = (j + 1) % raw_mask_len) raw[i] = raw[i] ^ raw_mask[j];
  return b64_format ? comm::base64_encode(raw) : raw;
}

static void SetIotmSignHeaders(HttpRequest* session, const std::string& body) {
  auto timestamp = std::to_string(comm::GetTimestampSec());
  auto nonce = cross::comm::GenRandomString(32, true);
  comm::Hmac<comm::SHA256> hmac(DecodeMixedSecret(kIotmSignSecret));
  hmac.Update(body);
  hmac.Update("\n" + timestamp + "\n");
  hmac.Update(nonce + "\n");
  auto sign = boost::algorithm::hex_lower(hmac.GetResult());
  session->UpdateHeader(HttpRequest::Header{
      {"IOT-KeyID", kIotmSignKeyId},
      {"IOT-TimeStamp", timestamp},
      {"IOT-NonceStr", nonce},
      {"IOT-Sign", sign},
  });
}

static constexpr char kIotmBaseUrl[] = "https://wxpay.iot.wechatpay.cn/iotmanage";
static constexpr char kIotDeviceDegradeRelativeUrl[] = "/iotpconn/degrade";

static InnerRequestRet IotManageRequestImpl(std::string relative_url, std::string data,
                                            std::chrono::milliseconds timeout) {
  auto executor = asio::this_coro::executor;  // 获取当前线程的执行器，如果不是协程，相当于当前的线程。
  auto url = kIotmBaseUrl + relative_url;
  LOG_I("iotm request url: {} timeout: {}", url, timeout);
  HttpRequest req;
  req.SetUrl({url});
  req.SetTimeout(timeout);
  req.SetHeader(HttpRequest::Header{{"Content-Type", "application/json"}});
  SetIotmSignHeaders(&req, data);
  req.SetBody(std::move(data));

  auto ret = req.PostSync();
  LOG_I("iotm request return, url: {} status_code: {} elapsed: {}", relative_url, ret.status_code, ret.elapsed);
  if (ret.status_code == 200) {
    LOG_I("receive:{}", ret.text);
    return InnerRequestRet(0, std::move(ret.text), {});
  }
  LOG_E("iotm http request fail, url: {} status_code: {} error: {} errmsg: {}", relative_url, ret.status_code,
        ret.error.code, ret.error.message);
  return InnerRequestRet(ret.error ? static_cast<int>(ret.error.code) : (ret.status_code ? ret.status_code : -1),
                         std::move(ret.text), std::move(ret.error.message));
}

static std::optional<std::string> GZipCompress(const std::string& data) {
  try {
    std::istringstream input_stream(data);
    std::ostringstream output_stream;
    boost::iostreams::filtering_streambuf<boost::iostreams::output> out;
    out.push(boost::iostreams::gzip_compressor(boost::iostreams::gzip::default_compression));
    out.push(output_stream);

    boost::iostreams::copy(input_stream, out);
    return output_stream.str();
  } catch (const std::exception& ex) {
    LOG_E("compress fail, exception: {}", ex.what());
    return std::nullopt;
  }
}

static std::optional<std::string> GZipDecompress(const std::string& compressed_data) {
  try {
    std::istringstream input_stream(compressed_data);
    std::ostringstream output_stream;
    boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
    in.push(boost::iostreams::gzip_decompressor());
    in.push(input_stream);

    boost::iostreams::copy(in, output_stream);
    return output_stream.str();
  } catch (const std::exception& ex) {
    LOG_E("decompress fail, exception: {}", ex.what());
    return std::nullopt;
  }
}

// ============================================================================
// HTTP响应通用处理函数
// ============================================================================

/**
 * @brief 判断是否是CPR网络请求错误
 * @param error_code CPR错误码
 * @return true表示是网络请求相关错误
 */
static bool IsCprNetworkError(int error_code) {
  // 明确的网络连接相关错误码
  return error_code == static_cast<int>(cpr::ErrorCode::CONNECTION_FAILURE) ||
         error_code == static_cast<int>(cpr::ErrorCode::HOST_RESOLUTION_FAILURE) ||
         error_code == static_cast<int>(cpr::ErrorCode::NETWORK_RECEIVE_ERROR) ||
         error_code == static_cast<int>(cpr::ErrorCode::NETWORK_SEND_FAILURE) ||
         error_code == static_cast<int>(cpr::ErrorCode::PROXY_RESOLUTION_FAILURE);
}

/**
 * @brief 解析IoT设备HTTP响应并转换为统一错误码
 * @param status_code HTTP状态码
 * @param error_obj HTTP错误对象(cpr::Error)
 * @param response_body 响应体内容
 * @return IoTResponse 包含错误码、服务端错误码和响应内容
 */
static Network::IoTResponse ParseIoTHttpResponse(
    int status_code,
    const cpr::Error& error_obj,
    std::string response_body) {

  // 网络层错误(连接失败、超时等)
  if (error_obj) {
    // 超时错误单独处理
    if (error_obj.code == cpr::ErrorCode::OPERATION_TIMEDOUT) {
      LOG_E("HTTP request timeout: {}", error_obj.message);
      return {ERR_PALM_IOT_HTTP_TIMEOUT, std::move(response_body), error_obj.message};
    }

    // 判断是否是CPR网络请求错误
    if (IsCprNetworkError(static_cast<int>(error_obj.code))) {
      // 再判断网络连接状态,区分返回网络未连接还是请求失败
      if (!NetworkInfo::Instance()->IsConnected()) {
        LOG_E("HTTP request failed and network is not connected: code={}, message={}",
              static_cast<int>(error_obj.code), error_obj.message);
        return {ERR_PALM_IOT_NETWORK_ERROR, std::move(response_body), error_obj.message};
      } else {
        LOG_E("HTTP request failed but network is connected (DNS/connection issue): code={}, message={}",
              static_cast<int>(error_obj.code), error_obj.message);
        return {ERR_PALM_IOT_HTTP_CLIENT_ERROR, std::move(response_body), error_obj.message};
      }
    }

    // 其他错误(SSL错误、请求取消等)
    LOG_E("HTTP error: code={}, message={}", static_cast<int>(error_obj.code), error_obj.message);
    return {ERR_PALM_IOT_HTTP_CLIENT_ERROR, std::move(response_body), error_obj.message};
  }

  // 统一尝试从响应体中提取业务错误码
  if (!response_body.empty()) {
    try {
      auto json_resp = nlohmann::json::parse(response_body);
      if (json_resp.contains("code")) {
        int code = json_resp["code"].get<int>();
        if (code != 0) {
          std::string message = json_resp.value("message", "");
          LOG_E("HTTP {} server returned error: code={}, message={}", status_code, code, message);
          return {ERR_PALM_IOT_HTTP_SERVER_ERROR, code, std::move(response_body), message};
        }
      }
    } catch (const std::exception& e) {
      LOG_E("HTTP {} response body JSON parse error: {}, body: {}", status_code, e.what(), response_body);
      // JSON解析失败不一定是错误，可能是protobuf等其他格式
    }
  }

  // HTTP 200 且无业务错误，返回成功
  if (status_code == 200) {
    return {ERR_PALM_IOT_SUCCESS, std::move(response_body)};
  }

  // HTTP 4xx 客户端错误
  if (status_code >= 400 && status_code < 500) {
    LOG_E("HTTP client error: status={}", status_code);
    return {ERR_PALM_IOT_HTTP_CLIENT_ERROR, std::move(response_body),
            fmt::format("HTTP client error: status={}", status_code)};
  }

  // HTTP 5xx 服务端错误
  if (status_code >= 500 && status_code < 600) {
    LOG_E("HTTP server error: status={}", status_code);
    return {ERR_PALM_IOT_HTTP_SERVER_ERROR, std::move(response_body),
            fmt::format("HTTP server error: status={}", status_code)};
  }

  // 其他未知HTTP错误
  LOG_E("HTTP unknown error: status={}", status_code);
  return {ERR_PALM_IOT_UNKNOWN_ERROR, std::move(response_body),
          fmt::format("HTTP unknown error: status={}", status_code)};
}

std::string Network::GenerateTraceparent(bool start_new_trace) {
  if (start_new_trace || current_trace_id_.empty()) {
    current_trace_id_ = utils::StringUtil::GenerateRandomHexString(32);
  }
  std::string span_id = utils::StringUtil::GenerateRandomHexString(16);
  return fmt::format("00-{}-{}-01", current_trace_id_, span_id);
}

// ============================================================================
// 激活签名相关辅助函数
// ============================================================================

// 签名上下文结构
struct SignContext {
  std::string nonce;       // 随机字符串
  std::string timestamp;   // 时间戳
  std::string sha256_hex;  // SHA256 哈希值(HEX 格式)
};

// 辅助函数: 构建签名原文(二进制方式)
static std::vector<uint8_t> BuildSignContent(const std::string& method,
                                             const std::string& path,
                                             const std::string& timestamp,
                                             const std::string& nonce,
                                             const std::vector<uint8_t>& body) {
  return utils::StringUtil::ByteMerge({
      utils::StringUtil::StringToBytes(method + "\n"),
      utils::StringUtil::StringToBytes(path + "\n"),
      utils::StringUtil::StringToBytes(timestamp + "\n"),
      utils::StringUtil::StringToBytes(nonce + "\n"),
      body,
      utils::StringUtil::StringToBytes("\n")
  });
}

// 辅助函数: 计算签名原文的 SHA256 哈希(HEX 格式)
static std::string ComputeSHA256Hex(const void* data, size_t size) {
  auto sha256_hash = cross::comm::DoDigest<cross::comm::SHA256>(data, size);
  return boost::algorithm::hex_lower(sha256_hash);
}

// 核心函数: 生成签名上下文(nonce + timestamp + sha256_hex)
static SignContext GenerateSignContext(const std::string& method,
                                       const std::string& path,
                                       const std::vector<uint8_t>& body) {
  SignContext ctx;
  ctx.nonce = cross::comm::GenRandomString(32, true);
  ctx.timestamp = std::to_string(comm::GetTimestampSec());

  // 构建签名原文并计算 SHA256
  auto content_bytes = BuildSignContent(method, path, ctx.timestamp, ctx.nonce, body);
  ctx.sha256_hex = ComputeSHA256Hex(content_bytes.data(), content_bytes.size());

  return ctx;
}

// 核心函数: 组装 Authorization Header
static std::string BuildAuthorizationHeader(const std::string& scheme,
                                           const std::string& signature,
                                           const std::string& nonce,
                                           const std::string& timestamp,
                                           const std::string& session_id = "") {
  std::string header = fmt::format(
      "{} device_sn=\"{}\",nonce_str=\"{}\",signature=\"{}\",timestamp=\"{}\"",
      scheme, Device::GetSn(), nonce, signature, timestamp);

  if (!session_id.empty()) {
    header += fmt::format(",session_id=\"{}\"", session_id);
  }

  return header;
}

// SM3-HMAC 实现(使用腾讯 SM 国密库)
static std::string SM3HMAC(const std::string& key, const std::string& data) {
  unsigned char mac[SM3_HMAC_SIZE];  // 32 字节

  int ret = SM3_HMAC(
      reinterpret_cast<const unsigned char*>(data.data()),
      data.size(),
      reinterpret_cast<const unsigned char*>(key.data()),
      key.size(),
      mac);

  if (ret != 0) {
    LOG_E("SM3_HMAC failed with error code: {}", ret);
    return "";
  }

  return std::string(reinterpret_cast<char*>(mac), SM3_HMAC_SIZE);
}

// 生成 SIMPLE-HMAC-SM3 签名
std::string Network::GenerateSimpleAuth(const std::string& method,
                                       const std::string& path,
                                       const std::string& body) {
  // 生成签名上下文
  auto ctx = GenerateSignContext(method, path, utils::StringUtil::StringToBytes(body));

  // SM3-HMAC 签名(使用固定密钥)
  auto hmac_result = SM3HMAC(kSimpleSignKey, ctx.sha256_hex);
  auto signature = comm::base64_encode(hmac_result);

  // 组装 Authorization Header
  return BuildAuthorizationHeader("SIMPLE-HMAC-SM3", signature, ctx.nonce, ctx.timestamp);
}

// 生成 ACTIVATION-HMAC-SM3 签名
std::string Network::GenerateActivationAuth(const std::string& method,
                                           const std::string& path,
                                           const std::vector<uint8_t>& body) {
  // 生成签名上下文
  auto ctx = GenerateSignContext(method, path, body);

  // SM3-HMAC 签名(使用会话密钥)
  auto hmac_result = SM3HMAC(session_key_, ctx.sha256_hex);
  auto signature = comm::base64_encode(hmac_result);

  // 组装 Authorization Header
  return BuildAuthorizationHeader("ACTIVATION-HMAC-SM3", signature, ctx.nonce, ctx.timestamp, session_id_);
}

// 生成 IOTDEVICE-SHA256-SM2 签名
std::string Network::GenerateDeviceAuth(const std::string& method,
                                       const std::string& path,
                                       const std::vector<uint8_t>& body) {
  // 生成签名上下文
  auto ctx = GenerateSignContext(method, path, body);

  // 根据 SeKeyStore 初始化状态选择签名方式
  std::string sign_base64;
  if (SeKeyStore::Instance()->IsInited()) {
    // SE 已初始化，使用 SeKeyStore 进行 SM2 签名
    LOG_I("IOTDEVICE-SHA256-SM2: Using SE for signature");
    sign_base64 = SeKeyStore::Instance()->SignDataHash(ctx.sha256_hex);
    if (sign_base64.empty()) {
      LOG_E("IOTDEVICE-SHA256-SM2: SeKeyStore SM2 signature failed");
      return "";
    }
    LOG_D("IOTDEVICE-SHA256-SM2: SE signature success");
  } else {
    // SE 未初始化，使用 PalmInterface 进行 SM2 签名
    LOG_I("IOTDEVICE-SHA256-SM2: Using PalmInterface service for signature");
    auto sha256_base64 = comm::base64_encode(ctx.sha256_hex);
    auto ec = PalmInterface::Instance()->SignCommonAuthSync(sha256_base64, false, sign_base64);
    if (ec) {
      LOG_E("IOTDEVICE-SHA256-SM2: PalmInterface SM2 signature failed, error: {}", ec.message());
      return "";
    }
    if (sign_base64.empty()) {
      LOG_E("IOTDEVICE-SHA256-SM2: PalmInterface SM2 signature is empty");
      return "";
    }
    LOG_D("IOTDEVICE-SHA256-SM2: PalmInterface signature success");
  }

  // 构造 PalmSignPackage protobuf 结构
  auto sign_bytes = comm::base64_decode(sign_base64);
  PalmSignPackage sign_pb;
  sign_pb.set_version("1");
  sign_pb.set_sign(sign_bytes.data(), sign_bytes.size());
  sign_pb.set_type("DeviceAuthSign");

  // 序列化并 Base64 编码
  std::string serialized = sign_pb.SerializeAsString();
  auto signature = comm::base64_encode(serialized);

  LOG_D("IOTDEVICE-SHA256-SM2: signature generated, length: {}", signature.size());

  // 组装 Authorization Header
  return BuildAuthorizationHeader("IOTDEVICE-SHA256-SM2", signature, ctx.nonce, ctx.timestamp, session_id_);
}

// IoT Device 请求通用实现(GET/POST复用)
Network::IoTResponse Network::IotDeviceRequestImpl(const std::string& method,
                                                    std::string path,
                                                    std::string body_data,
                                                    std::chrono::milliseconds timeout,
                                                    IoTSignType sign_type,
                                                    bool is_json_request) {
  comm::TimeCounter tc;
  int up_data_len = 0;
  int down_data_len = 0;
  int error_code = 0;
  std::string error_msg;

  SCOPE_EXIT(&path, &error_code, &error_msg, &tc, &up_data_len, &down_data_len) {
    NetworkStat::ReportDeviceRequest(true, path, error_code, error_msg, up_data_len, down_data_len, tc.ElapsedMs());
  }
  SCOPE_EXIT_END

  LOG_I("iot device {} request path: {} timeout: {} sign_type: {}",
        method, path, timeout, static_cast<int>(sign_type));

  // 检查域名是否设置
  CHECK_IOT_DEVICE_HOST_SET(error_code, error_msg);

  // 准备请求体
  up_data_len = body_data.length();
  if (!body_data.empty()) {
    LOG_D("iot device request body: {}", body_data);
  }

  // 根据签名类型生成签名
  auto body_bytes = utils::StringUtil::StringToBytes(body_data);
  std::string auth_header;

  switch (sign_type) {
    case IoTSignType::SIMPLE:
      auth_header = GenerateSimpleAuth(method, path, body_data);
      break;
    case IoTSignType::ACTIVATION:
      auth_header = GenerateActivationAuth(method, path, body_bytes);
      break;
    case IoTSignType::DEVICE:
      auth_header = GenerateDeviceAuth(method, path, body_bytes);
      break;
  }

  LOG_D("GenerateAuth result: {}", auth_header);
  if (auth_header.empty()) {
    LOG_E("iot device request generate auth failed");
    error_code = ERR_PALM_IOT_SIGN_FAILED;
    error_msg = "Generate auth signature failed";
    return {error_code, "", error_msg};
  }

  auto traceparent = GenerateTraceparent(true);

  // 构建 HTTP 请求
  std::string full_url = iot_device_host_ + path;
  HttpRequest http_req;
  http_req.SetUrl({full_url});
  http_req.SetTimeout(timeout);
  HttpRequest::Header request_headers{
      {"Authorization", auth_header},
      {"Content-Type", is_json_request ? "application/json" : "application/x-protobuf"},
      {"Accept", "application/json"},
      {"traceparent", traceparent}};

#ifndef NDEBUG
  // Add Zerus environment header in Debug builds
  const auto& zerus_env_id = palm::Application::GetArgs().zerus_env_id;
  if (zerus_env_id.has_value() && !zerus_env_id->empty()) {
    request_headers["X-Zerus-Env"] = *zerus_env_id;
    LOG_W("Add X-Zerus-Env header: {}", *zerus_env_id);
  }
#endif

  http_req.SetHeader(request_headers);

  LOG_I("iot device {} request, url: {} traceparent: {} timeout: {}", method, full_url, traceparent, timeout);

  // 打印请求headers
  LOG_D("iot device {} request headers:", method);
  for (const auto& [key, value] : request_headers) {
    LOG_D("  {}: {}", key, value);
  }

  // 发送请求
  HttpRequest::Response ret;
  if (method == "POST") {
    http_req.SetBody(std::move(body_data));
    ret = http_req.PostSync();
  } else if (method == "PUT") {
    http_req.SetBody(std::move(body_data));
    ret = http_req.PutSync();
  } else if (method == "GET") {
    ret = http_req.GetSync();
  } else {
    LOG_E("Unsupported HTTP method: {}", method);
    error_code = ERR_PALM_IOT_UNKNOWN_ERROR;
    error_msg = "Unsupported HTTP method";
    return {error_code, "", error_msg};
  }

  down_data_len = ret.text.length();

  LOG_I("iot device {} request return, url: {} status_code: {} elapsed: {}",
        method, path, ret.status_code, ret.elapsed);

  // 打印返回headers
  LOG_D("iot device {} response headers:", method);
  for (const auto& [key, value] : ret.header) {
    LOG_D("  {}: {}", key, value);
  }

  LOG_D("response body: {}", ret.text);

  // 使用通用函数解析HTTP响应
  auto response = ParseIoTHttpResponse(ret.status_code, ret.error, std::move(ret.text));

  // 更新 error_code 和 error_msg 用于统计上报
  error_code = response.error_code;
  if (response.server_code != 0) {
    error_msg = response.error_message.empty()
                ? fmt::format("Server error: code={}", response.server_code)
                : response.error_message;
  } else if (error_code != ERR_PALM_IOT_SUCCESS) {
    error_msg = response.error_message.empty()
                ? "Request failed"
                : response.error_message;
  }

  LOG_D("iot device {} request done, path: {} response len:{}",
        method, path, response.response_body.size());
  return response;
}

// IoT Device POST 请求(使用 IOTDEVICE-SHA256-SM2 签名，请求和返回都使用 JSON)
Network::IoTResponse Network::IotDeviceRequest(std::string path, std::string msg_data,
                                               std::chrono::milliseconds timeout,
                                               IoTSignType sign_type,
                                               bool use_compression,
                                               bool is_json_request) {
  return IotDeviceRequestImpl("POST", std::move(path), std::move(msg_data), timeout, sign_type, is_json_request);
}

// IoT Device GET 请求(使用 IOTDEVICE-SHA256-SM2 签名，返回使用 JSON)
Network::IoTResponse Network::IotDeviceGetRequest(std::string path,
                                                  const std::map<std::string, std::string>& query_params,
                                                  std::chrono::milliseconds timeout,
                                                  IoTSignType sign_type,
                                                  bool is_json_request) {
  // 构建查询参数
  if (!query_params.empty()) {
    std::vector<std::string> params;
    for (const auto& [key, value] : query_params) {
      params.push_back(fmt::format("{}={}", key, value));
    }
    std::ostringstream oss;
    for (size_t i = 0; i < params.size(); ++i) {
      if (i > 0) oss << "&";
      oss << params[i];
    }
    path += "?" + oss.str();
  }

  return IotDeviceRequestImpl("GET", std::move(path), "", timeout, sign_type, is_json_request);
}

// IoT Device PUT 请求(使用 IOTDEVICE-SHA256-SM2 签名，请求和返回都使用 JSON)
Network::IoTResponse Network::IotDevicePutRequest(std::string path, std::string msg_data,
                                                  std::chrono::milliseconds timeout,
                                                  IoTSignType sign_type,
                                                  bool use_compression,
                                                  bool is_json_request) {
  return IotDeviceRequestImpl("PUT", std::move(path), std::move(msg_data), timeout, sign_type, is_json_request);
}

std::pair<int32_t, std::string> Network::IotManageRequest(std::string relative_url, std::string data,
                                                          std::chrono::milliseconds timeout) {
  InnerRequestRet ret;

  comm::TimeCounter tc;
  int up_len = 0;
  int down_len = 0;
  SCOPE_EXIT(&relative_url, &ret, &tc, &up_len, &down_len) {
    NetworkStat::ReportDeviceRequest(true, relative_url, ret.error, ret.errmsg, up_len, down_len, tc.ElapsedMs());
  }
  SCOPE_EXIT_END
  up_len = data.length();
  ret = IotManageRequestImpl(relative_url, std::move(data), timeout);
  down_len = ret.resp_data.length();
  return std::pair<int32_t, std::string>(ret.error, std::move(ret.resp_data));
}

// ============================================================================
// 激活请求兼容接口 (内部调用 IotDevice* 系列接口)
// ============================================================================

Network::IoTResponse Network::ActivationGetRequest(
    const std::string& path,
    const std::map<std::string, std::string>& query_params,
    std::chrono::milliseconds timeout) {
  return IotDeviceGetRequest(path, query_params, timeout, IoTSignType::SIMPLE, true);
}

Network::IoTResponse Network::ActivationPostRequest(
    IoTSignType type,
    const std::string& path,
    const std::vector<uint8_t>& pb_body,
    bool start_new_trace,
    std::chrono::milliseconds timeout,
    bool is_json_request) {

  // 重置 trace ID 如果需要
  if (start_new_trace) {
    current_trace_id_.clear();
  }

  // 将二进制 pb_body 转换为字符串
  std::string body_str = utils::StringUtil::BytesToString(pb_body);

  return IotDeviceRequest(path, std::move(body_str), timeout, type, false, is_json_request);
}

}  // namespace palm
