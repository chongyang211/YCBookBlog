// Copyright © 1998 - 2024 Tencent. All Rights Reserved.

#pragma once

#include <string>
#include <vector>
#include "nlohmann/json.hpp"

namespace palm {
namespace entity {

// JSON key 常量定义（与服务端protobuf字段名保持一致）
namespace property_keys {
  // 通用字段
  constexpr const char* kDeviceSn = "device_sn";
  constexpr const char* kCurrVersions = "curr_versions";
  constexpr const char* kDeviceSideProperties = "device_side_properties";
  constexpr const char* kCloudSideProperties = "cloud_side_properties";
  constexpr const char* kAckCloudSideProperties = "ack_cloud_side_properties";
  constexpr const char* kErrorList = "error_list";

  // 属性字段
  constexpr const char* kPropertyIdentifier = "property_identifier";
  constexpr const char* kPropertyValue = "property_value";
  constexpr const char* kPropertyVersion = "property_version";
  constexpr const char* kPropertyTime = "property_time";
  constexpr const char* kScopeType = "scope_type";
  constexpr const char* kScopeValue = "scope_value";

  // ACK字段
  constexpr const char* kAckVersion = "ack_version";
  constexpr const char* kAckTime = "ack_time";

  // 错误字段
  constexpr const char* kErrorCode = "error_code";
  constexpr const char* kErrorMsg = "error_msg";
}  // namespace property_keys

// ========== 设备侧属性相关实体 ==========

// 设备侧属性版本信息
struct DeviceSidePropertyVersion {
  std::string property_identifier;
  uint32_t property_version = 0;
  uint32_t scope_type = 0;
  std::string scope_value;

  nlohmann::json ToJson() const {
    nlohmann::json json;
    json[property_keys::kPropertyIdentifier] = property_identifier;
    json[property_keys::kPropertyVersion] = property_version;
    json[property_keys::kScopeType] = scope_type;
    json[property_keys::kScopeValue] = scope_value;
    return json;
  }

  static DeviceSidePropertyVersion FromJson(const nlohmann::json& json) {
    DeviceSidePropertyVersion version;
    if (json.contains(property_keys::kPropertyIdentifier)) {
      version.property_identifier = json[property_keys::kPropertyIdentifier].get<std::string>();
    }
    if (json.contains(property_keys::kPropertyVersion)) {
      version.property_version = json[property_keys::kPropertyVersion].get<uint32_t>();
    }
    if (json.contains(property_keys::kScopeType)) {
      version.scope_type = json[property_keys::kScopeType].get<uint32_t>();
    }
    if (json.contains(property_keys::kScopeValue)) {
      version.scope_value = json[property_keys::kScopeValue].get<std::string>();
    }
    return version;
  }
};

struct DeviceSideProperty {
  std::string property_identifier;
  std::string property_value;
  uint32_t property_version = 0;
  std::string property_time;
  uint32_t scope_type = 0;
  std::string scope_value;

  nlohmann::json ToJson() const {
    nlohmann::json json;
    json[property_keys::kPropertyIdentifier] = property_identifier;
    json[property_keys::kPropertyValue] = property_value;
    json[property_keys::kPropertyVersion] = property_version;
    json[property_keys::kPropertyTime] = property_time;
    json[property_keys::kScopeType] = scope_type;
    json[property_keys::kScopeValue] = scope_value;
    return json;
  }

  static DeviceSideProperty FromJson(const nlohmann::json& json) {
    DeviceSideProperty property;
    if (json.contains(property_keys::kPropertyIdentifier)) {
      property.property_identifier = json[property_keys::kPropertyIdentifier].get<std::string>();
    }
    if (json.contains(property_keys::kPropertyValue)) {
      property.property_value = json[property_keys::kPropertyValue].get<std::string>();
    }
    if (json.contains(property_keys::kPropertyVersion)) {
      property.property_version = json[property_keys::kPropertyVersion].get<uint32_t>();
    }
    if (json.contains(property_keys::kPropertyTime)) {
      property.property_time = json[property_keys::kPropertyTime].get<std::string>();
    }
    if (json.contains(property_keys::kScopeType)) {
      property.scope_type = json[property_keys::kScopeType].get<uint32_t>();
    }
    if (json.contains(property_keys::kScopeValue)) {
      property.scope_value = json[property_keys::kScopeValue].get<std::string>();
    }
    return property;
  }
};

// 设备侧属性错误信息
struct DeviceSidePropertyError {
  std::string property_identifier;
  uint32_t property_version = 0;
  uint32_t error_code = 0;
  std::string error_msg;
  uint32_t scope_type = 0;
  std::string scope_value;

  nlohmann::json ToJson() const {
    nlohmann::json json;
    json[property_keys::kPropertyIdentifier] = property_identifier;
    json[property_keys::kPropertyVersion] = property_version;
    json[property_keys::kErrorCode] = error_code;
    json[property_keys::kErrorMsg] = error_msg;
    json[property_keys::kScopeType] = scope_type;
    json[property_keys::kScopeValue] = scope_value;
    return json;
  }

  static DeviceSidePropertyError FromJson(const nlohmann::json& json) {
    DeviceSidePropertyError error;
    if (json.contains(property_keys::kPropertyIdentifier)) {
      error.property_identifier = json[property_keys::kPropertyIdentifier].get<std::string>();
    }
    if (json.contains(property_keys::kPropertyVersion)) {
      error.property_version = json[property_keys::kPropertyVersion].get<uint32_t>();
    }
    if (json.contains(property_keys::kErrorCode)) {
      error.error_code = json[property_keys::kErrorCode].get<uint32_t>();
    }
    if (json.contains(property_keys::kErrorMsg)) {
      error.error_msg = json[property_keys::kErrorMsg].get<std::string>();
    }
    if (json.contains(property_keys::kScopeType)) {
      error.scope_type = json[property_keys::kScopeType].get<uint32_t>();
    }
    if (json.contains(property_keys::kScopeValue)) {
      error.scope_value = json[property_keys::kScopeValue].get<std::string>();
    }
    return error;
  }
};

// 获取设备侧属性差分请求
struct GetDeviceSidePropertiesDiffReq {
  std::string device_sn;
  std::vector<DeviceSidePropertyVersion> curr_versions;

  nlohmann::json ToJson() const {
    nlohmann::json json;
    json[property_keys::kDeviceSn] = device_sn;
    nlohmann::json versions_array = nlohmann::json::array();
    for (const auto& ver : curr_versions) {
      versions_array.push_back(ver.ToJson());
    }
    json[property_keys::kCurrVersions] = versions_array;
    return json;
  }

  static GetDeviceSidePropertiesDiffReq FromJson(const nlohmann::json& json) {
    GetDeviceSidePropertiesDiffReq req;
    if (json.contains(property_keys::kDeviceSn)) {
      req.device_sn = json[property_keys::kDeviceSn].get<std::string>();
    }
    if (json.contains(property_keys::kCurrVersions) && json[property_keys::kCurrVersions].is_array()) {
      for (const auto& item : json[property_keys::kCurrVersions]) {
        req.curr_versions.push_back(DeviceSidePropertyVersion::FromJson(item));
      }
    }
    return req;
  }
};

// 获取设备侧属性差分响应
struct GetDeviceSidePropertiesDiffResp {
  int32_t code = 0;
  std::string message;
  std::vector<DeviceSideProperty> device_side_properties;

  nlohmann::json ToJson() const {
    nlohmann::json json;
    nlohmann::json properties_array = nlohmann::json::array();
    for (const auto& prop : device_side_properties) {
      properties_array.push_back(prop.ToJson());
    }
    json[property_keys::kDeviceSideProperties] = properties_array;
    return json;
  }

  static GetDeviceSidePropertiesDiffResp FromJson(const nlohmann::json& json) {
    GetDeviceSidePropertiesDiffResp resp;
    if (json.contains(property_keys::kDeviceSideProperties) && json[property_keys::kDeviceSideProperties].is_array()) {
      for (const auto& item : json[property_keys::kDeviceSideProperties]) {
        resp.device_side_properties.push_back(DeviceSideProperty::FromJson(item));
      }
    }
    return resp;
  }
};

// 更新设备侧属性请求
struct UpdateDeviceSidePropertiesReq {
  std::string device_sn;
  std::vector<DeviceSideProperty> device_side_properties;

  nlohmann::json ToJson() const {
    nlohmann::json json;
    json[property_keys::kDeviceSn] = device_sn;
    nlohmann::json properties_array = nlohmann::json::array();
    for (const auto& prop : device_side_properties) {
      properties_array.push_back(prop.ToJson());
    }
    json[property_keys::kDeviceSideProperties] = properties_array;
    return json;
  }

  static UpdateDeviceSidePropertiesReq FromJson(const nlohmann::json& json) {
    UpdateDeviceSidePropertiesReq req;
    if (json.contains(property_keys::kDeviceSn)) {
      req.device_sn = json[property_keys::kDeviceSn].get<std::string>();
    }
    if (json.contains(property_keys::kDeviceSideProperties) && json[property_keys::kDeviceSideProperties].is_array()) {
      for (const auto& item : json[property_keys::kDeviceSideProperties]) {
        req.device_side_properties.push_back(DeviceSideProperty::FromJson(item));
      }
    }
    return req;
  }
};

// 更新设备侧属性响应
struct UpdateDeviceSidePropertiesResp {
  int32_t code = 0;
  std::string message;
  std::vector<DeviceSidePropertyError> error_list;

  nlohmann::json ToJson() const {
    nlohmann::json json;
    nlohmann::json errors_array = nlohmann::json::array();
    for (const auto& err : error_list) {
      errors_array.push_back(err.ToJson());
    }
    json[property_keys::kErrorList] = errors_array;
    return json;
  }

  static UpdateDeviceSidePropertiesResp FromJson(const nlohmann::json& json) {
    UpdateDeviceSidePropertiesResp resp;
    if (json.contains(property_keys::kErrorList) && json[property_keys::kErrorList].is_array()) {
      for (const auto& item : json[property_keys::kErrorList]) {
        resp.error_list.push_back(DeviceSidePropertyError::FromJson(item));
      }
    }
    return resp;
  }
};

// ========== 云端侧属性相关实体 ==========

// 云端侧属性版本信息
struct CloudSidePropertyVersion {
  std::string property_identifier;
  uint32_t property_version = 0;
  uint32_t scope_type = 0;
  std::string scope_value;

  nlohmann::json ToJson() const {
    nlohmann::json json;
    json[property_keys::kPropertyIdentifier] = property_identifier;
    json[property_keys::kPropertyVersion] = property_version;
    json[property_keys::kScopeType] = scope_type;
    json[property_keys::kScopeValue] = scope_value;
    return json;
  }

  static CloudSidePropertyVersion FromJson(const nlohmann::json& json) {
    CloudSidePropertyVersion version;
    if (json.contains(property_keys::kPropertyIdentifier)) {
      version.property_identifier = json[property_keys::kPropertyIdentifier].get<std::string>();
    }
    if (json.contains(property_keys::kPropertyVersion)) {
      version.property_version = json[property_keys::kPropertyVersion].get<uint32_t>();
    }
    if (json.contains(property_keys::kScopeType)) {
      version.scope_type = json[property_keys::kScopeType].get<uint32_t>();
    }
    if (json.contains(property_keys::kScopeValue)) {
      version.scope_value = json[property_keys::kScopeValue].get<std::string>();
    }
    return version;
  }
};

// 云端侧属性完整信息
struct CloudSideProperty {
  std::string property_identifier;
  std::string property_value;
  uint32_t property_version = 0;
  std::string property_time;
  uint32_t ack_version = 0;
  std::string ack_time;
  uint32_t scope_type = 0;
  std::string scope_value;

  nlohmann::json ToJson() const {
    nlohmann::json json;
    json[property_keys::kPropertyIdentifier] = property_identifier;
    json[property_keys::kPropertyValue] = property_value;
    json[property_keys::kPropertyVersion] = property_version;
    json[property_keys::kPropertyTime] = property_time;
    json[property_keys::kAckVersion] = ack_version;
    json[property_keys::kAckTime] = ack_time;
    json[property_keys::kScopeType] = scope_type;
    json[property_keys::kScopeValue] = scope_value;
    return json;
  }

  static CloudSideProperty FromJson(const nlohmann::json& json) {
    CloudSideProperty property;
    if (json.contains(property_keys::kPropertyIdentifier)) {
      property.property_identifier = json[property_keys::kPropertyIdentifier].get<std::string>();
    }
    if (json.contains(property_keys::kPropertyValue)) {
      property.property_value = json[property_keys::kPropertyValue].get<std::string>();
    }
    if (json.contains(property_keys::kPropertyVersion)) {
      property.property_version = json[property_keys::kPropertyVersion].get<uint32_t>();
    }
    if (json.contains(property_keys::kPropertyTime)) {
      property.property_time = json[property_keys::kPropertyTime].get<std::string>();
    }
    if (json.contains(property_keys::kAckVersion)) {
      property.ack_version = json[property_keys::kAckVersion].get<uint32_t>();
    }
    if (json.contains(property_keys::kAckTime)) {
      property.ack_time = json[property_keys::kAckTime].get<std::string>();
    }
    if (json.contains(property_keys::kScopeType)) {
      property.scope_type = json[property_keys::kScopeType].get<uint32_t>();
    }
    if (json.contains(property_keys::kScopeValue)) {
      property.scope_value = json[property_keys::kScopeValue].get<std::string>();
    }
    return property;
  }
};

// 获取云端侧属性差分请求
struct GetCloudSidePropertiesDiffReq {
  std::string device_sn;
  std::vector<CloudSidePropertyVersion> curr_versions;

  nlohmann::json ToJson() const {
    nlohmann::json json;
    json[property_keys::kDeviceSn] = device_sn;
    nlohmann::json versions_array = nlohmann::json::array();
    for (const auto& ver : curr_versions) {
      versions_array.push_back(ver.ToJson());
    }
    json[property_keys::kCurrVersions] = versions_array;
    return json;
  }

  static GetCloudSidePropertiesDiffReq FromJson(const nlohmann::json& json) {
    GetCloudSidePropertiesDiffReq req;
    if (json.contains(property_keys::kDeviceSn)) {
      req.device_sn = json[property_keys::kDeviceSn].get<std::string>();
    }
    if (json.contains(property_keys::kCurrVersions) && json[property_keys::kCurrVersions].is_array()) {
      for (const auto& item : json[property_keys::kCurrVersions]) {
        req.curr_versions.push_back(CloudSidePropertyVersion::FromJson(item));
      }
    }
    return req;
  }
};

// 获取云端侧属性差分响应
struct GetCloudSidePropertiesDiffResp {
  int32_t code = 0;
  std::string message;
  std::vector<CloudSideProperty> cloud_side_properties;

  nlohmann::json ToJson() const {
    nlohmann::json json;
    nlohmann::json properties_array = nlohmann::json::array();
    for (const auto& prop : cloud_side_properties) {
      properties_array.push_back(prop.ToJson());
    }
    json[property_keys::kCloudSideProperties] = properties_array;
    return json;
  }

  static GetCloudSidePropertiesDiffResp FromJson(const nlohmann::json& json) {
    GetCloudSidePropertiesDiffResp resp;
    if (json.contains(property_keys::kCloudSideProperties) && json[property_keys::kCloudSideProperties].is_array()) {
      for (const auto& item : json[property_keys::kCloudSideProperties]) {
        resp.cloud_side_properties.push_back(CloudSideProperty::FromJson(item));
      }
    }
    return resp;
  }
};

// 云端侧属性ACK信息
struct AcknowledgeCloudSideProperty {
  std::string property_identifier;
  uint32_t ack_version = 0;
  uint32_t error_code = 0;
  std::string error_msg;
  uint32_t scope_type = 0;
  std::string scope_value;

  nlohmann::json ToJson() const {
    nlohmann::json json;
    json[property_keys::kPropertyIdentifier] = property_identifier;
    json[property_keys::kAckVersion] = ack_version;
    json[property_keys::kErrorCode] = error_code;
    json[property_keys::kErrorMsg] = error_msg;
    json[property_keys::kScopeType] = scope_type;
    json[property_keys::kScopeValue] = scope_value;
    return json;
  }

  static AcknowledgeCloudSideProperty FromJson(const nlohmann::json& json) {
    AcknowledgeCloudSideProperty ack;
    if (json.contains(property_keys::kPropertyIdentifier)) {
      ack.property_identifier = json[property_keys::kPropertyIdentifier].get<std::string>();
    }
    if (json.contains(property_keys::kAckVersion)) {
      ack.ack_version = json[property_keys::kAckVersion].get<uint32_t>();
    }
    if (json.contains(property_keys::kErrorCode)) {
      ack.error_code = json[property_keys::kErrorCode].get<uint32_t>();
    }
    if (json.contains(property_keys::kErrorMsg)) {
      ack.error_msg = json[property_keys::kErrorMsg].get<std::string>();
    }
    if (json.contains(property_keys::kScopeType)) {
      ack.scope_type = json[property_keys::kScopeType].get<uint32_t>();
    }
    if (json.contains(property_keys::kScopeValue)) {
      ack.scope_value = json[property_keys::kScopeValue].get<std::string>();
    }
    return ack;
  }
};

// 云端侧属性ACK错误信息
struct AcknowledgeCloudSidePropertyError {
  std::string property_identifier;
  uint32_t ack_version = 0;
  uint32_t error_code = 0;
  std::string error_msg;
  uint32_t scope_type = 0;
  std::string scope_value;

  nlohmann::json ToJson() const {
    nlohmann::json json;
    json[property_keys::kPropertyIdentifier] = property_identifier;
    json[property_keys::kAckVersion] = ack_version;
    json[property_keys::kErrorCode] = error_code;
    json[property_keys::kErrorMsg] = error_msg;
    json[property_keys::kScopeType] = scope_type;
    json[property_keys::kScopeValue] = scope_value;
    return json;
  }

  static AcknowledgeCloudSidePropertyError FromJson(const nlohmann::json& json) {
    AcknowledgeCloudSidePropertyError error;
    if (json.contains(property_keys::kPropertyIdentifier)) {
      error.property_identifier = json[property_keys::kPropertyIdentifier].get<std::string>();
    }
    if (json.contains(property_keys::kAckVersion)) {
      error.ack_version = json[property_keys::kAckVersion].get<uint32_t>();
    }
    if (json.contains(property_keys::kErrorCode)) {
      error.error_code = json[property_keys::kErrorCode].get<uint32_t>();
    }
    if (json.contains(property_keys::kErrorMsg)) {
      error.error_msg = json[property_keys::kErrorMsg].get<std::string>();
    }
    if (json.contains(property_keys::kScopeType)) {
      error.scope_type = json[property_keys::kScopeType].get<uint32_t>();
    }
    if (json.contains(property_keys::kScopeValue)) {
      error.scope_value = json[property_keys::kScopeValue].get<std::string>();
    }
    return error;
  }
};

// 确认云端侧属性请求
struct AcknowledgeCloudSidePropertiesReq {
  std::string device_sn;
  std::vector<AcknowledgeCloudSideProperty> ack_cloud_side_properties;

  nlohmann::json ToJson() const {
    nlohmann::json json;
    json[property_keys::kDeviceSn] = device_sn;
    nlohmann::json acks_array = nlohmann::json::array();
    for (const auto& ack : ack_cloud_side_properties) {
      acks_array.push_back(ack.ToJson());
    }
    json[property_keys::kAckCloudSideProperties] = acks_array;
    return json;
  }

  static AcknowledgeCloudSidePropertiesReq FromJson(const nlohmann::json& json) {
    AcknowledgeCloudSidePropertiesReq req;
    if (json.contains(property_keys::kDeviceSn)) {
      req.device_sn = json[property_keys::kDeviceSn].get<std::string>();
    }
    if (json.contains(property_keys::kAckCloudSideProperties) && json[property_keys::kAckCloudSideProperties].is_array()) {
      for (const auto& item : json[property_keys::kAckCloudSideProperties]) {
        req.ack_cloud_side_properties.push_back(AcknowledgeCloudSideProperty::FromJson(item));
      }
    }
    return req;
  }
};

// 确认云端侧属性响应
struct AcknowledgeCloudSidePropertiesResp {
  int32_t code = 0;
  std::string message;
  std::vector<AcknowledgeCloudSidePropertyError> error_list;

  nlohmann::json ToJson() const {
    nlohmann::json json;
    nlohmann::json errors_array = nlohmann::json::array();
    for (const auto& err : error_list) {
      errors_array.push_back(err.ToJson());
    }
    json[property_keys::kErrorList] = errors_array;
    return json;
  }

  static AcknowledgeCloudSidePropertiesResp FromJson(const nlohmann::json& json) {
    AcknowledgeCloudSidePropertiesResp resp;
    if (json.contains(property_keys::kErrorList) && json[property_keys::kErrorList].is_array()) {
      for (const auto& item : json[property_keys::kErrorList]) {
        resp.error_list.push_back(AcknowledgeCloudSidePropertyError::FromJson(item));
      }
    }
    return resp;
  }
};

}  // namespace entity
}  // namespace palm
