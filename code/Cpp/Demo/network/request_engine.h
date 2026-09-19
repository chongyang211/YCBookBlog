// Copyright © 1998 - 2023 Tencent. All Rights Reserved.

#pragma once

#include <string>
#include <vector>
#include "network/entity/instruction_entity.h"
#include "network/entity/instruction_presign_entity.h"
#include "network/entity/opentelemetry_entity.h"
#include "network/entity/property_entity.h"
#include "network/entity/upgrade_entity.h"
#include "network/entity/heartbeat_entity.h"
#include "network/network.h"

namespace palm {

/**
 * @brief 网络请求引擎类，提供所有后台接口的统一封装
 */
class RequestEngine {
 public:
  static RequestEngine* Instance();
  ~RequestEngine();

 private:
  RequestEngine();

 public:
  /**
   * @brief 查询Palm固件注册信息
   * @param provider_code 供应商代码
   * @param hardware_model 硬件型号
   * @param firmware_id 返回的固件ID
   * @param plat_application_id 返回的平台应用ID
   * @return IoTResponse 响应结构，error_code为0表示成功
   */
  Network::IoTResponse QueryPalmFirmwareReg(const std::string& provider_code,
                                            const std::string& hardware_model,
                                            uint32_t& firmware_id,
                                            std::string& plat_application_id);

  /**
   * @brief 查询Palm子应用注册信息
   * @param app_id 应用ID
   * @param package_name 包名
   * @param app_signature 应用签名
   * @param sub_app_scene 子应用场景
   * @param sub_app_id 返回的子应用ID
   * @param plat_application_id 返回的平台应用ID
   * @return IoTResponse 响应结构，error_code为0表示成功
   */
  Network::IoTResponse QueryPalmSubAppReg(const std::string& app_id,
                                          const std::string& package_name,
                                          const std::string& app_signature,
                                          const std::string& sub_app_scene,
                                          uint32_t& sub_app_id,
                                          std::string& plat_application_id);

  /**
   * @brief 查询Palm ROM注册信息
   * @param rom_model ROM型号
   * @param rom_id 返回的ROM ID
   * @param plat_application_id 返回的平台应用ID
   * @return IoTResponse 响应结构，error_code为0表示成功
   */
  Network::IoTResponse QueryPalmRomReg(const std::string& rom_model,
                                       uint32_t& rom_id,
                                       std::string& plat_application_id);

  /**
   * @brief OTA升级轮询查询
   * @param app_versions 设备软件版本信息列表
   * @param upgrade_details 返回的升级明细列表
   * @return IoTResponse 响应结构，error_code为0表示成功
   */
  Network::IoTResponse BatchQueryUpgradeDetail(
      const std::vector<entity::AppVersion>& app_versions,
      std::vector<entity::DeviceUpgradeExecDetail>& upgrade_details);

  /**
   * @brief 查询设备指令执行列表
   * @param instructions 返回的指令列表
   * @return IoTResponse 响应结构，error_code为0表示成功
   */
  Network::IoTResponse QueryInstructionExecuteList(std::vector<entity::Instruction>& instructions);

  /**
   * @brief 更新指令执行状态
   * @param req 更新指令执行状态请求
   * @return IoTResponse 响应结构，error_code为0表示成功
   */
  Network::IoTResponse UpdateInstructionExecState(const entity::UpdateInstructionExecStateReq& req);

  /**
   * @brief 上报日志数据 (OpenTelemetry Logs)
   * @param req 日志上报请求
   * @return IoTResponse 响应结构，error_code为0表示成功
   */
  Network::IoTResponse ReportLogs(const entity::ExportLogsServiceReq& req);

  /**
   * @brief 上报指标数据 (OpenTelemetry Metrics)
   * @param req 指标上报请求
   * @return IoTResponse 响应结构，error_code为0表示成功
   */
  Network::IoTResponse ReportMetrics(const entity::ExportMetricsServiceReq& req);

  /**
   * @brief 上报追踪数据 (OpenTelemetry Trace)
   * @param req 追踪上报请求
   * @return IoTResponse 响应结构，error_code为0表示成功
   */
  Network::IoTResponse ReportTrace(const entity::ExportTraceServiceReq& req);

  /**
   * @brief 生成指令预签名信息（获取COS上传/下载URL）
   * @param req 预签名信息请求
   * @param upload_url 返回的上传URL
   * @param download_url 返回的下载URL
   * @param content_type 返回的Content-Type
   * @return IoTResponse 响应结构，error_code为0表示成功
   */
  Network::IoTResponse GenerateInstructionPresignInfo(
      const entity::GenerateInstructionPresignInfoReq& req,
      std::string& upload_url,
      std::string& download_url,
      std::string& content_type);

  // ========== 设备属性管理接口 ==========

  /**
   * @brief 获取设备侧属性差分
   * @param curr_versions 当前设备本地的属性版本列表
   * @param diff_properties 返回的差分属性列表
   * @return IoTResponse 响应结构，error_code为0表示成功
   */
  Network::IoTResponse GetDeviceSidePropertiesDiff(
      const std::vector<entity::DeviceSidePropertyVersion>& curr_versions,
      std::vector<entity::DeviceSideProperty>& diff_properties);

  /**
   * @brief 更新设备侧属性
   * @param properties 要更新的设备侧属性列表
   * @return IoTResponse 响应结构，error_code为0表示成功
   */
  Network::IoTResponse UpdateDeviceSideProperties(
      const std::vector<entity::DeviceSideProperty>& properties);

  /**
   * @brief 获取云端侧属性差分
   * @param curr_versions 当前设备本地的属性版本列表
   * @param diff_properties 返回的差分属性列表
   * @return IoTResponse 响应结构，error_code为0表示成功
   */
  Network::IoTResponse GetCloudSidePropertiesDiff(
      const std::vector<entity::CloudSidePropertyVersion>& curr_versions,
      std::vector<entity::CloudSideProperty>& diff_properties);

  /**
   * @brief 确认云端侧属性
   * @param ack_properties 要确认的云端侧属性ACK列表
   * @return IoTResponse 响应结构，error_code为0表示成功
   */
  Network::IoTResponse AcknowledgeCloudSideProperties(
      const std::vector<entity::AcknowledgeCloudSideProperty>& ack_properties);

  // ========== 设备心跳接口 ==========

  /**
   * @brief 发送设备心跳请求
   * @param req 设备心跳请求
   * @param resp 返回的设备心跳响应
   * @return IoTResponse 响应结构，error_code为0表示成功
   */
  Network::IoTResponse SendDeviceHeartbeat(const entity::DeviceHeartbeatReq& req,
                                          entity::DeviceHeartbeatResp& resp);

  // ========== Action Code Interfaces ==========

  /**
   * @brief Fetch platform common key for action code signature verification.
   *        Supports MMKV cache and retry on failure.
   * @param public_key Output: the platform public key
   * @param use_cache Whether to use cached key (default true)
   * @return IoTResponse, error_code=0 means success
   */
  Network::IoTResponse FetchCommonKey(std::string& public_key, bool use_cache = true);

  /**
   * @brief Get cached common key without network request.
   *        For offline scenarios (e.g. medium-risk codes).
   * @param public_key Output: the cached public key
   * @return true if cache hit, false if no cached key
   */
  bool GetCachedCommonKey(std::string& public_key);

  /**
   * @brief Preload and cache the platform common key for action code signature verification.
   *        Should be called asynchronously during action code module initialization.
   */
  void PreloadCommonKey();

  /**
   * @brief Fetch high-risk action code detail from server.
   *        Server will delete the key after retrieval (one-time use).
   * @param verify_code The verify code extracted from action code payload
   * @param action_code_str Output: the full action code string
   * @return IoTResponse, error_code=0 means success
   */
  Network::IoTResponse FetchActionCodeDetail(const std::string& verify_code,
                                             std::string& action_code_str);

 private:
  // Retry wrapper for common key fetch
  Network::IoTResponse FetchCommonKeyFromServer(std::string& public_key);
};

}  // namespace palm
