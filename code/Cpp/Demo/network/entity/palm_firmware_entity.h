// Copyright © 1998 - 2020 Tencent. All Rights Reserved.

#pragma once

#include <string>
#include "nlohmann/json.hpp"

namespace palm {
namespace entity {

// JSON key 常量定义
namespace palm_firmware_keys {
  // QueryPalmFirmwareRegReq 字段
  constexpr const char* kProviderCode = "provider_code";
  constexpr const char* kHardwareModel = "hardware_model";

  // QueryPalmFirmwareRegResp 字段
  constexpr const char* kFirmwareId = "firmware_id";
  constexpr const char* kPlatApplicationId = "plat_application_id";
}  // namespace palm_firmware_keys

// QueryPalmFirmwareReg 请求
struct QueryPalmFirmwareRegReq {
  std::string provider_code;
  std::string hardware_model;

  nlohmann::json ToJson() const {
    nlohmann::json json;
    json[palm_firmware_keys::kProviderCode] = provider_code;
    json[palm_firmware_keys::kHardwareModel] = hardware_model;
    return json;
  }

  static QueryPalmFirmwareRegReq FromJson(const nlohmann::json& json) {
    QueryPalmFirmwareRegReq req;
    if (json.contains(palm_firmware_keys::kProviderCode)) {
      req.provider_code = json[palm_firmware_keys::kProviderCode].get<std::string>();
    }
    if (json.contains(palm_firmware_keys::kHardwareModel)) {
      req.hardware_model = json[palm_firmware_keys::kHardwareModel].get<std::string>();
    }
    return req;
  }
};

// QueryPalmFirmwareReg 响应
struct QueryPalmFirmwareRegResp {
  uint32_t firmware_id = 0;
  std::string plat_application_id;

  nlohmann::json ToJson() const {
    nlohmann::json json;
    json[palm_firmware_keys::kFirmwareId] = firmware_id;
    json[palm_firmware_keys::kPlatApplicationId] = plat_application_id;
    return json;
  }

  static QueryPalmFirmwareRegResp FromJson(const nlohmann::json& json) {
    QueryPalmFirmwareRegResp resp;
    if (json.contains(palm_firmware_keys::kFirmwareId)) {
      resp.firmware_id = json[palm_firmware_keys::kFirmwareId].get<uint32_t>();
    }
    if (json.contains(palm_firmware_keys::kPlatApplicationId)) {
      resp.plat_application_id = json[palm_firmware_keys::kPlatApplicationId].get<std::string>();
    }
    return resp;
  }
};

}  // namespace entity
}  // namespace palm
