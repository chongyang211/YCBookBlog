// Copyright © 1998 - 2023 Tencent. All Rights Reserved.

#pragma once

#include <map>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "comm/comm.h"

#include "app/app.h"

namespace palm {

class Network {
 public:
  static Network* Instance();
  ~Network();

 private:
  Network();

 public:
  struct TdiResponse {
    int32_t return_code = -1;
    std::string business_data;
  };

  /**
   * @brief IoT设备请求响应结构
   */
  struct IoTResponse {
    int32_t error_code = 0;      // 统一错误码(0表示成功)
    int32_t server_code = 0;     // 服务端业务错误码(仅当HTTP 200但业务失败时有效)
    std::string response_body;   // 响应体内容
    std::string error_message;   // 错误信息描述

    IoTResponse() = default;
    IoTResponse(int32_t ec, std::string body)
        : error_code(ec), response_body(std::move(body)) {}
    IoTResponse(int32_t ec, int32_t sc, std::string body)
        : error_code(ec), server_code(sc), response_body(std::move(body)) {}
    IoTResponse(int32_t ec, std::string body, std::string err_msg)
        : error_code(ec), response_body(std::move(body)), error_message(std::move(err_msg)) {}
    IoTResponse(int32_t ec, int32_t sc, std::string body, std::string err_msg)
        : error_code(ec), server_code(sc), response_body(std::move(body)), error_message(std::move(err_msg)) {}
  };

  /**
   * @brief IoT请求签名类型
   */
  enum class IoTSignType {
    SIMPLE,      // SIMPLE-HMAC-SM3: 基础签名,使用固定密钥
    ACTIVATION,  // ACTIVATION-HMAC-SM3: 激活流程签名,使用会话密钥
    DEVICE       // IOTDEVICE-SHA256-SM2: 设备签名,使用 SE 安全模块
  };

  // todo TDI 双链路请求（iLink + HTTPS短链接）, 这里未实现
  static std::pair<int32_t, TdiResponse> DualLinkTdiRequest(uint32_t cmdid, std::string relative_url,
                                                            std::chrono::milliseconds timeout, bool for_pay,
                                                            std::string session_id, std::string biz_data);

  /**
   * @brief 设置IoT设备服务域名
   * @param domain IoT服务域名
   */
  void SetIotDeviceHost(const std::string& domain);

  /**
   * @brief 获取IoT设备服务域名
   * @return IoT域名,未设置则返回空字符串
   */
  std::string GetIotDeviceHost() const { return iot_device_host_; }

  /**
   * @brief 设置激活会话信息
   * @param domain IoT服务域名
   * @param session_id 会话ID
   * @param session_key 会话密钥(用于 ACTIVATION 类型签名)
   */
  void SetActivationSession(const std::string& domain,
                            const std::string& session_id,
                            const std::string& session_key);

  /**
   * @brief 清理激活会话信息
   * @note 激活成功后调用，清除临时的session_id和session_key
   */
  void ClearActivationSession();

  /**
   * @brief IoT Device POST 请求
   * @param path 请求路径
   * @param msg_data 消息数据(JSON格式或二进制数据)
   * @param timeout 请求超时时间(默认15秒)
   * @param sign_type 签名类型(默认使用 DEVICE 签名)
   * @param use_compression 是否使用压缩(默认 false)
   * @param is_json_request 请求体是否为JSON格式(默认 true, false则为protobuf)
   * @return IoTResponse 包含错误码、服务端错误码和响应内容
   */
  IoTResponse IotDeviceRequest(std::string path,
                               std::string msg_data,
                               std::chrono::milliseconds timeout = std::chrono::seconds(15),
                               IoTSignType sign_type = IoTSignType::DEVICE,
                               bool use_compression = false,
                               bool is_json_request = true);

  /**
   * @brief IoT Device GET 请求
   * @param path 请求路径(可包含查询参数)
   * @param query_params 查询参数(默认为空)
   * @param timeout 请求超时时间(默认15秒)
   * @param sign_type 签名类型(默认使用 DEVICE 签名)
   * @param is_json_request 请求体是否为JSON格式(默认 true, false则为protobuf)
   * @return IoTResponse 包含错误码、服务端错误码和响应内容
   */
  IoTResponse IotDeviceGetRequest(std::string path,
                                  const std::map<std::string, std::string>& query_params = {},
                                  std::chrono::milliseconds timeout = std::chrono::seconds(15),
                                  IoTSignType sign_type = IoTSignType::DEVICE,
                                  bool is_json_request = true);

  /**
   * @brief IoT Device PUT 请求
   * @param path 请求路径
   * @param msg_data 消息数据(JSON格式)
   * @param timeout 请求超时时间(默认15秒)
   * @param sign_type 签名类型(默认使用 DEVICE 签名)
   * @param use_compression 是否使用压缩(默认 false)
   * @param is_json_request 请求体是否为JSON格式(默认 true, false则为protobuf)
   * @return IoTResponse 包含错误码、服务端错误码和响应内容
   */
  IoTResponse IotDevicePutRequest(std::string path,
                                  std::string msg_data,
                                  std::chrono::milliseconds timeout = std::chrono::seconds(15),
                                  IoTSignType sign_type = IoTSignType::DEVICE,
                                  bool use_compression = false,
                                  bool is_json_request = true);

  /**
   * @brief 激活 GET 请求 (兼容接口，内部调用 IotDeviceGetRequest)
   * @param path 请求路径
   * @param query_params 查询参数
   * @param timeout 请求超时时间(默认15秒)
   * @return IoTResponse 包含错误码、服务端错误码和响应内容
   */
  IoTResponse ActivationGetRequest(const std::string& path,
                                   const std::map<std::string, std::string>& query_params,
                                   std::chrono::milliseconds timeout = std::chrono::seconds(15));

  /**
   * @brief 激活 POST 请求 (兼容接口，内部调用 IotDeviceRequest)
   * @param type 签名类型
   * @param path 请求路径
   * @param pb_body Protobuf 序列化后的请求体
   * @param timeout 请求超时时间(默认15秒)
   * @param is_json_request 请求体是否为JSON格式(默认 false, 使用protobuf)
   * @param start_new_trace 是否开始新的 trace
   * @return IoTResponse 包含错误码、服务端错误码和响应内容
   */
  IoTResponse ActivationPostRequest(IoTSignType type,
                                    const std::string& path,
                                    const std::vector<uint8_t>& pb_body,
                                    bool start_new_trace = false,
                                    std::chrono::milliseconds timeout = std::chrono::seconds(15),
                                    bool is_json_request = false);

  // Iot平台(Iot Manage)请求
  std::pair<int32_t, std::string> IotManageRequest(std::string relative_url, std::string data,
                                                   std::chrono::milliseconds timeout);

 private:
  /**
   * @brief 生成 SIMPLE-HMAC-SM3 签名
   * @param method HTTP 方法
   * @param path 请求路径
   * @param body 请求体(字符串)
   * @return Authorization Header 值
   */
  std::string GenerateSimpleAuth(const std::string& method,
                                 const std::string& path,
                                 const std::string& body);

  /**
   * @brief 生成 ACTIVATION-HMAC-SM3 签名
   * @param method HTTP 方法
   * @param path 请求路径
   * @param body 请求体(二进制)
   * @return Authorization Header 值
   */
  std::string GenerateActivationAuth(const std::string& method,
                                     const std::string& path,
                                     const std::vector<uint8_t>& body);

  /**
   * @brief 生成 IOTDEVICE-SHA256-SM2 签名
   * @param method HTTP 方法
   * @param path 请求路径
   * @param body 请求体(二进制)
   * @return Authorization Header 值
   */
  std::string GenerateDeviceAuth(const std::string& method,
                                 const std::string& path,
                                 const std::vector<uint8_t>& body);

  /**
   * @brief 生成 traceparent header
   * @param start_new_trace 是否开始新的 trace
   * @return traceparent header 值
   */
  std::string GenerateTraceparent(bool start_new_trace);

  /**
   * @brief IoT Device 请求通用实现(GET/POST/PUT复用)
   * @param method HTTP 方法("GET"、"POST"或"PUT")
   * @param path 请求路径
   * @param body_data 请求体数据(GET请求为空)
   * @param timeout 请求超时时间
   * @param sign_type 签名类型
   * @param is_json_request 请求体是否为JSON格式(false则为protobuf)
   * @return IoTResponse 包含错误码、服务端错误码和响应内容
   */
  IoTResponse IotDeviceRequestImpl(const std::string& method,
                                   std::string path,
                                   std::string body_data,
                                   std::chrono::milliseconds timeout,
                                   IoTSignType sign_type,
                                   bool is_json_request);

  // IoT设备会话信息(用于签名)
  std::string iot_device_host_;   // IoT服务域名(构建 URL)
  std::string session_id_;        // 会话ID(ACTIVATION/DEVICE 签名)
  std::string session_key_;       // 会话密钥(ACTIVATION 签名)
  std::string current_trace_id_;  // 当前 trace ID(用于分布式追踪)
};
}  // namespace palm
