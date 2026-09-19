// Copyright © 1998 - 2020 Tencent. All Rights Reserved.

#include "network/request_engine.h"

#include <chrono>
#include <thread>

#include "comm/constant/unified_errcode_iot.h"
#include "config/config.h"
#include "cross/comm/logger.h"
#include "device/device.h"
#include "network/entity/action_code_entity.h"
#include "network/entity/instruction_entity.h"
#include "network/entity/instruction_presign_entity.h"
#include "network/entity/opentelemetry_entity.h"
#include "network/entity/palm_firmware_entity.h"
#include "network/entity/palm_rom_entity.h"
#include "network/entity/palm_sub_app_entity.h"
#include "network/entity/upgrade_entity.h"
#include "network/entity/heartbeat_entity.h"
#include "network/network.h"
#include "nlohmann/json.hpp"

#include "cross/comm/base64.h"

namespace palm {

RequestEngine::RequestEngine() = default;

RequestEngine::~RequestEngine() = default;

RequestEngine* RequestEngine::Instance() {
  static RequestEngine inst;
  return &inst;
}

Network::IoTResponse RequestEngine::QueryPalmFirmwareReg(const std::string& provider_code,
                                                          const std::string& hardware_model,
                                                          uint32_t& firmware_id,
                                                          std::string& plat_application_id) {
  entity::QueryPalmFirmwareRegReq req;
  req.provider_code = provider_code;
  req.hardware_model = hardware_model;

  std::string req_data = req.ToJson().dump();
  LOG_I("QueryPalmFirmwareReg request: {}", req.ToJson().dump());

  auto response = Network::Instance()->IotDeviceRequest(
      "/iot-palm/query-palm-firmware-reg",
      req_data,
      std::chrono::milliseconds(5000));

  if (response.error_code != 0) {
    LOG_E("QueryPalmFirmwareReg failed, error_code: {}, server_code: {}, "
          "error_msg: {}, request: {}, response: {}",
          response.error_code, response.server_code, response.error_message,
          req.ToJson().dump(), response.response_body);
    return response;
  }

  try {
    auto resp_json = nlohmann::json::parse(response.response_body);
    auto resp = entity::QueryPalmFirmwareRegResp::FromJson(resp_json);
    LOG_I("QueryPalmFirmwareReg response: {}", resp.ToJson().dump());

    firmware_id = resp.firmware_id;
    plat_application_id = resp.plat_application_id;

    LOG_I("QueryPalmFirmwareReg success, firmware_id: {}, plat_application_id: {}",
          firmware_id, plat_application_id);

    return response;
  } catch (const std::exception& ex) {
    LOG_E("QueryPalmFirmwareReg parse response failed, exception: {}, request: {}, response: {}",
          ex.what(), req.ToJson().dump(), response.response_body);
    return Network::IoTResponse(ERR_PALM_IOT_PARSE_ERROR, response.response_body, ex.what());
  }
}

Network::IoTResponse RequestEngine::QueryPalmSubAppReg(const std::string& app_id,
                                                        const std::string& package_name,
                                                        const std::string& app_signature,
                                                        const std::string& sub_app_scene,
                                                        uint32_t& sub_app_id,
                                                        std::string& plat_application_id) {
  entity::QueryPalmSubAppRegReq req;
  req.app_id = app_id;
  req.package_name = package_name;
  req.app_signature = app_signature;
  req.sub_app_scene = sub_app_scene;

  std::string req_data = req.ToJson().dump();
  LOG_I("QueryPalmSubAppReg request: {}", req.ToJson().dump());

  auto response = Network::Instance()->IotDeviceRequest(
      "/iot-palm/query-palm-sub-app-reg",
      req_data,
      std::chrono::milliseconds(5000));

  if (response.error_code != 0) {
    LOG_E("QueryPalmSubAppReg failed, error_code: {}, server_code: {}, "
          "error_msg: {}, request: {}, response: {}",
          response.error_code, response.server_code, response.error_message,
          req.ToJson().dump(), response.response_body);
    return response;
  }

  try {
    auto resp_json = nlohmann::json::parse(response.response_body);
    auto resp = entity::QueryPalmSubAppRegResp::FromJson(resp_json);
    LOG_I("QueryPalmSubAppReg response: {}", resp.ToJson().dump());

    sub_app_id = resp.sub_app_id;
    plat_application_id = resp.plat_application_id;

    LOG_I("QueryPalmSubAppReg success, sub_app_id: {}, plat_application_id: {}",
          sub_app_id, plat_application_id);

    return response;
  } catch (const std::exception& ex) {
    LOG_E("QueryPalmSubAppReg parse response failed, exception: {}, request: {}, response: {}",
          ex.what(), req.ToJson().dump(), response.response_body);
    return Network::IoTResponse(ERR_PALM_IOT_PARSE_ERROR, response.response_body, ex.what());
  }
}

Network::IoTResponse RequestEngine::QueryPalmRomReg(const std::string& rom_model,
                                                     uint32_t& rom_id,
                                                     std::string& plat_application_id) {
  entity::QueryPalmRomRegReq req;
  req.rom_model = rom_model;

  std::string req_data = req.ToJson().dump();
  LOG_I("QueryPalmRomReg request: {}", req.ToJson().dump());

  auto response = Network::Instance()->IotDeviceRequest(
      "/iot-palm/query-palm-rom-reg",
      req_data,
      std::chrono::milliseconds(5000));

  if (response.error_code != 0) {
    LOG_E("QueryPalmRomReg failed, error_code: {}, server_code: {}, "
          "error_msg: {}, request: {}, response: {}",
          response.error_code, response.server_code, response.error_message,
          req.ToJson().dump(), response.response_body);
    return response;
  }

  try {
    auto resp_json = nlohmann::json::parse(response.response_body);
    auto resp = entity::QueryPalmRomRegResp::FromJson(resp_json);
    LOG_I("QueryPalmRomReg response: {}", resp.ToJson().dump());

    rom_id = resp.rom_id;
    plat_application_id = resp.plat_application_id;

    LOG_I("QueryPalmRomReg success, rom_id: {}, plat_application_id: {}",
          rom_id, plat_application_id);

    return response;
  } catch (const std::exception& ex) {
    LOG_E("QueryPalmRomReg parse response failed, exception: {}, request: {}, response: {}",
          ex.what(), req.ToJson().dump(), response.response_body);
    return Network::IoTResponse(ERR_PALM_IOT_PARSE_ERROR, response.response_body, ex.what());
  }
}

Network::IoTResponse RequestEngine::BatchQueryUpgradeDetail(
    const std::vector<entity::AppVersion>& app_versions,
    std::vector<entity::DeviceUpgradeExecDetail>& upgrade_details) {
  std::string device_sn = Device::GetSn();
  if (device_sn.empty()) {
    LOG_E("BatchQueryUpgradeDetail failed, device_sn is empty");
    return Network::IoTResponse(ERR_PALM_IOT_DEVICE_SN_EMPTY, "", "device_sn is empty");
  }

  entity::BatchQueryUpgradeDetailReq req;
  req.device_sn = device_sn;
  req.app_versions = app_versions;

  std::string req_data = req.ToJson().dump();
  LOG_I("BatchQueryUpgradeDetail request: {}", req.ToJson().dump());

  std::string path = "/iot/devices/" + device_sn + "/upgradedetails/batch-query";

  auto response = Network::Instance()->IotDeviceRequest(
      path,
      req_data,
      std::chrono::milliseconds(5000));

  if (response.error_code != 0) {
    LOG_E("BatchQueryUpgradeDetail failed, error_code: {}, server_code: {}, "
          "error_msg: {}, request: {}, response: {}",
          response.error_code, response.server_code, response.error_message,
          req.ToJson().dump(), response.response_body);
    return response;
  }

  try {
    auto resp_json = nlohmann::json::parse(response.response_body);
    auto resp = entity::BatchQueryUpgradeDetailResp::FromJson(resp_json);
    LOG_I("BatchQueryUpgradeDetail response: {}", resp.ToJson().dump());

    upgrade_details = resp.device_upgrade_exec_details;

    LOG_I("BatchQueryUpgradeDetail success, upgrade_details count: {}",
          upgrade_details.size());

    return response;
  } catch (const std::exception& ex) {
    LOG_E("BatchQueryUpgradeDetail parse response failed, exception: {}, request: {}, response: {}",
          ex.what(), req.ToJson().dump(), response.response_body);
    return Network::IoTResponse(ERR_PALM_IOT_PARSE_ERROR, response.response_body, ex.what());
  }
}

Network::IoTResponse RequestEngine::QueryInstructionExecuteList(
    std::vector<entity::Instruction>& instructions) {
  std::string device_sn = Device::GetSn();
  if (device_sn.empty()) {
    LOG_E("QueryInstructionExecuteList failed, device_sn is empty");
    return Network::IoTResponse(ERR_PALM_IOT_DEVICE_SN_EMPTY, "", "device_sn is empty");
  }

  std::string path = "/iot/devices/" + device_sn + "/instructions/execute-list";

  LOG_I("QueryInstructionExecuteList request path: {}", path);

  auto response = Network::Instance()->IotDeviceGetRequest(path);

  if (response.error_code != 0) {
    LOG_E("QueryInstructionExecuteList failed, error_code: {}, server_code: {}, "
          "error_msg: {}, response: {}",
          response.error_code, response.server_code,
          response.error_message, response.response_body);
    return response;
  }

  try {
    auto resp_json = nlohmann::json::parse(response.response_body);
    auto resp = entity::QueryInstructionExecuteListResp::FromJson(resp_json);
    LOG_I("QueryInstructionExecuteList response: {}", resp.ToJson().dump());

    instructions = resp.instructions;

    LOG_I("QueryInstructionExecuteList success, instructions count: {}",
          instructions.size());

    return response;
  } catch (const std::exception& ex) {
    LOG_E("QueryInstructionExecuteList parse response failed, exception: {}, "
          "response: {}",
          ex.what(), response.response_body);
    return Network::IoTResponse(ERR_PALM_IOT_PARSE_ERROR, response.response_body, ex.what());
  }
}

Network::IoTResponse RequestEngine::UpdateInstructionExecState(
    const entity::UpdateInstructionExecStateReq& req) {
  std::string req_data = req.ToJson().dump();
  LOG_I("UpdateInstructionExecState request: {}", req.ToJson().dump());

  std::string path = "/iot/devices/" + req.device_sn + "/instructions/" +
                     req.instruction_id + "/update-exec-state";

  auto response = Network::Instance()->IotDeviceRequest(
      path,
      req_data,
      std::chrono::milliseconds(5000));

  if (response.error_code != 0) {
    LOG_E("UpdateInstructionExecState failed, error_code: {}, server_code: {}, "
          "error_msg: {}, request: {}, response: {}",
          response.error_code, response.server_code,
          response.error_message, req.ToJson().dump(), response.response_body);
    return response;
  }

  try {
    auto resp_json = nlohmann::json::parse(response.response_body);
    auto resp = entity::UpdateInstructionExecStateResp::FromJson(resp_json);
    LOG_I("UpdateInstructionExecState response: {}", resp.ToJson().dump());
  } catch (const std::exception& ex) {
    LOG_I("UpdateInstructionExecState success");
  }
  return response;
}

Network::IoTResponse RequestEngine::ReportLogs(const entity::ExportLogsServiceReq& req) {
  std::string req_data = req.ToJson().dump();
  LOG_D("ReportLogs request size: {} bytes, resource_logs count: {}",
        req_data.size(), req.resource_logs.size());

  auto response = Network::Instance()->IotDeviceRequest(
      "/iot/report/logs",
      req_data,
      std::chrono::milliseconds(5000));

  if (response.error_code != 0) {
    LOG_E("ReportLogs failed, error_code: {}, server_code: {}, "
          "error_msg: {}, request size: {} bytes, response: {}",
          response.error_code, response.server_code,
          response.error_message, req_data.size(), response.response_body);
    return response;
  }

  try {
    auto resp_json = nlohmann::json::parse(response.response_body);
    auto resp = entity::ExportLogsServiceResp::FromJson(resp_json);
    LOG_D("ReportLogs response: {}", resp.ToJson().dump());

    return response;
  } catch (const std::exception& ex) {
    LOG_E("ReportLogs parse response failed, exception: {}, response: {}",
          ex.what(), response.response_body);
    return Network::IoTResponse(ERR_PALM_IOT_PARSE_ERROR, response.response_body, ex.what());
  }
}

Network::IoTResponse RequestEngine::ReportMetrics(const entity::ExportMetricsServiceReq& req) {
  std::string req_data = req.ToJson().dump();
  LOG_D("ReportMetrics request size: {} bytes, resource_metrics count: {}",
        req_data.size(), req.resource_metrics.size());

  auto response = Network::Instance()->IotDeviceRequest(
      "/iot/report/metrics",
      req_data,
      std::chrono::milliseconds(5000));

  if (response.error_code != 0) {
    LOG_E("ReportMetrics failed, error_code: {}, server_code: {}, "
          "error_msg: {}, request size: {} bytes, response: {}",
          response.error_code, response.server_code,
          response.error_message, req_data.size(), response.response_body);
    return response;
  }

  try {
    auto resp_json = nlohmann::json::parse(response.response_body);
    auto resp = entity::ExportMetricsServiceResp::FromJson(resp_json);
    LOG_D("ReportMetrics response: {}", resp.ToJson().dump());

    return response;
  } catch (const std::exception& ex) {
    LOG_E("ReportMetrics parse response failed, exception: {}, response: {}",
          ex.what(), response.response_body);
    return Network::IoTResponse(ERR_PALM_IOT_PARSE_ERROR, response.response_body, ex.what());
  }
}

Network::IoTResponse RequestEngine::ReportTrace(const entity::ExportTraceServiceReq& req) {
  std::string req_data = req.ToJson().dump();
  LOG_D("ReportTrace request size: {} bytes, resource_spans count: {}",
        req_data.size(), req.resource_spans.size());

  auto response = Network::Instance()->IotDeviceRequest(
      "/iot/report/trace",
      req_data,
      std::chrono::milliseconds(5000));

  if (response.error_code != 0) {
    LOG_E("ReportTrace failed, error_code: {}, server_code: {}, "
          "error_msg: {}, request size: {} bytes, response: {}",
          response.error_code, response.server_code,
          response.error_message, req_data.size(), response.response_body);
    return response;
  }

  try {
    auto resp_json = nlohmann::json::parse(response.response_body);
    auto resp = entity::ExportTraceServiceResp::FromJson(resp_json);
    LOG_D("ReportTrace response: {}", resp.ToJson().dump());

    return response;
  } catch (const std::exception& ex) {
    LOG_E("ReportTrace parse response failed, exception: {}, response: {}",
          ex.what(), response.response_body);
    return Network::IoTResponse(ERR_PALM_IOT_PARSE_ERROR, response.response_body, ex.what());
  }
}

Network::IoTResponse RequestEngine::GenerateInstructionPresignInfo(
    const entity::GenerateInstructionPresignInfoReq& req,
    std::string& upload_url,
    std::string& download_url,
    std::string& content_type) {
  std::string req_data = req.ToJson().dump();
  LOG_I("GenerateInstructionPresignInfo request: {}", req.ToJson().dump());

  auto response = Network::Instance()->IotDeviceRequest(
      "/iot-palm/instruction/cos/presign",
      req_data,
      std::chrono::milliseconds(5000));

  if (response.error_code != 0) {
    LOG_E("GenerateInstructionPresignInfo failed, error_code: {}, "
          "server_code: {}, error_msg: {}, request: {}, response: {}",
          response.error_code, response.server_code,
          response.error_message, req.ToJson().dump(),
          response.response_body);
    return response;
  }

  try {
    auto resp_json = nlohmann::json::parse(response.response_body);
    auto resp = entity::GenerateInstructionPresignInfoResp::FromJson(
        resp_json);
    LOG_I("GenerateInstructionPresignInfo response: {}",
          resp.ToJson().dump());

    upload_url = resp.upload_url;
    download_url = resp.download_url;
    content_type = resp.content_type;

    LOG_I("GenerateInstructionPresignInfo success, upload_url: {}, "
          "download_url: {}, content_type: {}",
          upload_url, download_url, content_type);

    return response;
  } catch (const std::exception& ex) {
    LOG_E("GenerateInstructionPresignInfo parse response failed, "
          "exception: {}, request: {}, response: {}",
          ex.what(), req.ToJson().dump(), response.response_body);
    return Network::IoTResponse(ERR_PALM_IOT_PARSE_ERROR, response.response_body, ex.what());
  }
}

// ========== 设备属性管理接口实现 ==========

Network::IoTResponse RequestEngine::GetDeviceSidePropertiesDiff(
    const std::vector<entity::DeviceSidePropertyVersion>& curr_versions,
    std::vector<entity::DeviceSideProperty>& diff_properties) {

  std::string device_sn = Device::GetSn();
  if (device_sn.empty()) {
    LOG_E("GetDeviceSidePropertiesDiff failed, device_sn is empty");
    return Network::IoTResponse(ERR_PALM_IOT_DEVICE_SN_EMPTY, "", "device_sn is empty");
  }

  entity::GetDeviceSidePropertiesDiffReq req;
  req.device_sn = device_sn;
  req.curr_versions = curr_versions;

  std::string req_data = req.ToJson().dump();

  LOG_I("GetDeviceSidePropertiesDiff request: device_sn={}, properties_count={}",
        device_sn, curr_versions.size());

  std::string path = "/iot/devices/" + device_sn + "/device-side-properties/diff";

  auto response = Network::Instance()->IotDeviceRequest(
      path,
      req_data,
      std::chrono::milliseconds(5000));

  if (response.error_code != 0) {
    LOG_E("GetDeviceSidePropertiesDiff failed, error_code: {}, server_code: {}, "
          "error_msg: {}, request: {}, response: {}",
          response.error_code, response.server_code, response.error_message,
          req_data, response.response_body);
    return response;
  }

  try {
    auto resp_json = nlohmann::json::parse(response.response_body);
    auto resp = entity::GetDeviceSidePropertiesDiffResp::FromJson(resp_json);

    diff_properties = resp.device_side_properties;

    LOG_I("GetDeviceSidePropertiesDiff success, diff_properties_count: {}",
          diff_properties.size());

    return response;
  } catch (const std::exception& ex) {
    LOG_E("GetDeviceSidePropertiesDiff parse response failed, exception: {}, "
          "request: {}, response: {}",
          ex.what(), req_data, response.response_body);
    return Network::IoTResponse(ERR_PALM_IOT_PARSE_ERROR, response.response_body, ex.what());
  }
}

Network::IoTResponse RequestEngine::UpdateDeviceSideProperties(
    const std::vector<entity::DeviceSideProperty>& properties) {

  std::string device_sn = Device::GetSn();
  if (device_sn.empty()) {
    LOG_E("UpdateDeviceSideProperties failed, device_sn is empty");
    return Network::IoTResponse(ERR_PALM_IOT_DEVICE_SN_EMPTY, "", "device_sn is empty");
  }

  entity::UpdateDeviceSidePropertiesReq req;
  req.device_sn = device_sn;
  req.device_side_properties = properties;

  std::string req_data = req.ToJson().dump();

  LOG_I("UpdateDeviceSideProperties request: device_sn={}, properties_count={}",
        device_sn, properties.size());

  std::string path = "/iot/devices/" + device_sn + "/device-side-properties";

  auto response = Network::Instance()->IotDevicePutRequest(
      path,
      req_data,
      std::chrono::milliseconds(5000));

  if (response.error_code != 0) {
    LOG_E("UpdateDeviceSideProperties failed, error_code: {}, server_code: {}, "
          "error_msg: {}, request: {}, response: {}",
          response.error_code, response.server_code, response.error_message,
          req_data, response.response_body);
    return response;
  }

  LOG_I("UpdateDeviceSideProperties success");
  return response;
}

Network::IoTResponse RequestEngine::GetCloudSidePropertiesDiff(
    const std::vector<entity::CloudSidePropertyVersion>& curr_versions,
    std::vector<entity::CloudSideProperty>& diff_properties) {

  std::string device_sn = Device::GetSn();
  if (device_sn.empty()) {
    LOG_E("GetCloudSidePropertiesDiff failed, device_sn is empty");
    return Network::IoTResponse(ERR_PALM_IOT_DEVICE_SN_EMPTY, "", "device_sn is empty");
  }

  entity::GetCloudSidePropertiesDiffReq req;
  req.device_sn = device_sn;
  req.curr_versions = curr_versions;

  std::string req_data = req.ToJson().dump();

  LOG_I("GetCloudSidePropertiesDiff request: device_sn={}, properties_count={}",
        device_sn, curr_versions.size());

  std::string path = "/iot/devices/" + device_sn + "/cloud-side-properties/diff";

  auto response = Network::Instance()->IotDeviceRequest(
      path,
      req_data,
      std::chrono::milliseconds(5000));

  if (response.error_code != 0) {
    LOG_E("GetCloudSidePropertiesDiff failed, error_code: {}, server_code: {}, "
          "error_msg: {}, request: {}, response: {}",
          response.error_code, response.server_code, response.error_message,
          req_data, response.response_body);
    return response;
  }

  try {
    auto resp_json = nlohmann::json::parse(response.response_body);
    auto resp = entity::GetCloudSidePropertiesDiffResp::FromJson(resp_json);

    diff_properties = resp.cloud_side_properties;

    LOG_I("GetCloudSidePropertiesDiff success, diff_properties_count: {}",
          diff_properties.size());

    return response;
  } catch (const std::exception& ex) {
    LOG_E("GetCloudSidePropertiesDiff parse response failed, exception: {}, "
          "request: {}, response: {}",
          ex.what(), req_data, response.response_body);
    return Network::IoTResponse(ERR_PALM_IOT_PARSE_ERROR, response.response_body, ex.what());
  }
}

Network::IoTResponse RequestEngine::AcknowledgeCloudSideProperties(
    const std::vector<entity::AcknowledgeCloudSideProperty>& ack_properties) {

  std::string device_sn = Device::GetSn();
  if (device_sn.empty()) {
    LOG_E("AcknowledgeCloudSideProperties failed, device_sn is empty");
    return Network::IoTResponse(ERR_PALM_IOT_DEVICE_SN_EMPTY, "", "device_sn is empty");
  }

  entity::AcknowledgeCloudSidePropertiesReq req;
  req.device_sn = device_sn;
  req.ack_cloud_side_properties = ack_properties;

  std::string req_data = req.ToJson().dump();

  LOG_I("AcknowledgeCloudSideProperties request: device_sn={}, properties_count={}",
        device_sn, ack_properties.size());

  std::string path = "/iot/devices/" + device_sn + "/ack-cloud-side-properties";

  auto response = Network::Instance()->IotDeviceRequest(
      path,
      req_data,
      std::chrono::milliseconds(5000));

  if (response.error_code != 0) {
    LOG_E("AcknowledgeCloudSideProperties failed, error_code: {}, server_code: {}, "
          "error_msg: {}, request: {}, response: {}",
          response.error_code, response.server_code, response.error_message,
          req_data, response.response_body);
    return response;
  }

  LOG_I("AcknowledgeCloudSideProperties success");
  return response;
}


// 设备心跳接口实现
Network::IoTResponse RequestEngine::SendDeviceHeartbeat(const entity::DeviceHeartbeatReq& req,
                                                        entity::DeviceHeartbeatResp& resp) {
  std::string req_data = req.ToJson().dump();
  LOG_D("SendDeviceHeartbeat request: {}", req_data);

  // 构建请求路径: /iot/devices/{device_sn}/heartbeat
  std::string path = "/iot/devices/" + req.device_sn + "/heartbeat";
  
  auto response = Network::Instance()->IotDeviceRequest(
      path,
      req_data,
      std::chrono::milliseconds(5000));

  if (response.error_code != 0) {
    LOG_E("SendDeviceHeartbeat failed, error_code: {}, server_code: {}, "
          "error_msg: {}, request: {}, response: {}",
          response.error_code, response.server_code, response.error_message,
          req_data, response.response_body);
    return response;
  }

  try {
    auto resp_json = nlohmann::json::parse(response.response_body);
    resp = entity::DeviceHeartbeatResp::FromJson(resp_json);
    LOG_D("SendDeviceHeartbeat success, server_time: {}, heartbeat_interval: {}s",
          resp.server_time, resp.heartbeat_interval_sec);

    return response;
  } catch (const std::exception& ex) {
    LOG_E("SendDeviceHeartbeat parse response failed, exception: {}, request: {}, response: {}",
          ex.what(), req_data, response.response_body);
    response.error_code = ERR_PALM_IOT_PARSE_ERROR;
    response.error_message = "Failed to parse heartbeat response: " + std::string(ex.what());
    return response;
  }
}

// ========== Action Code Interfaces ==========

// TODO: 后续会统一到请求重试机制中，目前先单独实现重试逻辑
namespace {
constexpr const char* kCommonKeyCacheKey = "action_code_common_key";
constexpr int kFetchCommonKeyMaxRetries = 3;
constexpr int kFetchCommonKeyBaseDelayMs = 1000;

// Only retry on network/timeout/server errors
bool IsRetryableError(int error_code) {
  return error_code == ERR_PALM_IOT_NETWORK_ERROR ||
         error_code == ERR_PALM_IOT_HTTP_TIMEOUT ||
         error_code == ERR_PALM_IOT_HTTP_SERVER_ERROR;
}
}  // namespace

Network::IoTResponse RequestEngine::FetchCommonKeyFromServer(std::string& public_key) {
  std::string path = "/wecardpalm/device/get-device-common-key";

  Network::IoTResponse last_response;
  for (int attempt = 0; attempt < kFetchCommonKeyMaxRetries; ++attempt) {
    if (attempt > 0) {
      int delay_ms = kFetchCommonKeyBaseDelayMs * (1 << (attempt - 1));
      LOG_W("FetchCommonKey retry attempt {}/{}, delay {}ms", attempt + 1, kFetchCommonKeyMaxRetries, delay_ms);
      std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
    }

    last_response = Network::Instance()->IotDeviceRequest(
        path, "{}", std::chrono::milliseconds(5000));

    if (last_response.error_code == 0) {
      break;
    }
    if (!IsRetryableError(last_response.error_code)) {
      break;
    }
  }

  if (last_response.error_code != 0) {
    LOG_E("FetchCommonKey failed after retries, error_code: {}, error_msg: {}",
          last_response.error_code, last_response.error_message);
    return Network::IoTResponse(ERR_PALM_ACTION_CODE_FETCH_KEY_FAILED,
                                last_response.response_body,
                                last_response.error_message);
  }

  try {
    auto resp_json = nlohmann::json::parse(last_response.response_body);
    auto resp = entity::FetchCommonKeyResp::FromJson(resp_json);
    if (resp.public_key.empty()) {
      LOG_E("FetchCommonKey response public_key is empty");
      return Network::IoTResponse(ERR_PALM_ACTION_CODE_FETCH_KEY_FAILED, "", "public_key is empty");
    }
    // Base64 decode to get PEM format public key
    public_key = cross::comm::base64_decode_copy(resp.public_key);
    if (public_key.empty()) {
      LOG_E("FetchCommonKey base64 decode failed");
      return Network::IoTResponse(ERR_PALM_ACTION_CODE_FETCH_KEY_FAILED, "", "base64 decode failed");
    }
    LOG_I("FetchCommonKey success");
    return last_response;
  } catch (const std::exception& ex) {
    LOG_E("FetchCommonKey parse response failed, exception: {}, response: {}",
          ex.what(), last_response.response_body);
    return Network::IoTResponse(ERR_PALM_IOT_PARSE_ERROR, last_response.response_body, ex.what());
  }
}

bool RequestEngine::GetCachedCommonKey(std::string& public_key) {
  auto cached = Config::Global()->Value<std::string>(kCommonKeyCacheKey);
  if (cached.has_value() && !cached.value().empty()) {
    public_key = cached.value();
    return true;
  }
  return false;
}

Network::IoTResponse RequestEngine::FetchCommonKey(std::string& public_key, bool use_cache) {
  // Try cache first
  if (use_cache) {
    if (GetCachedCommonKey(public_key)) {
      LOG_I("FetchCommonKey using cached key");
      return Network::IoTResponse();
    }
  }

  // Cache miss or disabled, fetch from server
  auto response = FetchCommonKeyFromServer(public_key);
  if (response.error_code == 0) {
    // Cache persistently (key rarely changes)
    Config::Global()->SetValue(kCommonKeyCacheKey, public_key);
  }
  return response;
}

void RequestEngine::PreloadCommonKey() {
  std::string public_key;
  auto response = FetchCommonKeyFromServer(public_key);
  if (response.error_code == 0) {
    Config::Global()->SetValue(kCommonKeyCacheKey, public_key);
    LOG_I("PreloadCommonKey cached successfully");
  } else {
    LOG_W("PreloadCommonKey failed, will retry on scan, error_code: {}", response.error_code);
  }
}

Network::IoTResponse RequestEngine::FetchActionCodeDetail(const std::string& verify_code,
                                                          std::string& action_code_str) {
  entity::FetchActionCodeDetailReq req;
  req.verify_code = verify_code;
  std::string req_data = req.ToJson().dump();

  std::string path = "/wecardpalm/device/get-action-code-detail";

  auto response = Network::Instance()->IotDeviceRequest(
      path, req_data, std::chrono::milliseconds(5000));

  if (response.error_code != 0) {
    LOG_E("FetchActionCodeDetail failed, error_code: {}, server_code: {}, error_msg: {}",
          response.error_code, response.server_code, response.error_message);
    return Network::IoTResponse(ERR_PALM_ACTION_CODE_FETCH_DETAIL_FAILED,
                                response.response_body,
                                response.error_message);
  }

  try {
    auto resp_json = nlohmann::json::parse(response.response_body);
    auto resp = entity::FetchActionCodeDetailResp::FromJson(resp_json);
    if (resp.action_code_str.empty()) {
      LOG_E("FetchActionCodeDetail response action_code_str is empty");
      return Network::IoTResponse(ERR_PALM_ACTION_CODE_FETCH_DETAIL_FAILED, "", "action_code_str is empty");
    }
    action_code_str = resp.action_code_str;
    LOG_I("FetchActionCodeDetail success");
    return response;
  } catch (const std::exception& ex) {
    LOG_E("FetchActionCodeDetail parse response failed, exception: {}, response: {}",
          ex.what(), response.response_body);
    return Network::IoTResponse(ERR_PALM_IOT_PARSE_ERROR, response.response_body, ex.what());
  }
}

}  // namespace palm
