// Copyright © 1998 - 2024 Tencent. All Rights Reserved.

#pragma once

#include <string>
#include "nlohmann/json.hpp"

namespace palm {
namespace entity {

// JSON key 常量定义（与服务端protobuf字段名保持一致）
namespace heartbeat_keys {
  // 请求字段
  constexpr const char* kDeviceSn = "device_sn";
  constexpr const char* kTimestamp = "timestamp";
  
  // 响应字段
  constexpr const char* kServerTime = "server_time";
  constexpr const char* kHeartbeatIntervalSec = "heartbeat_interval_sec";
  constexpr const char* kNeedConfigSync = "need_config_sync";
}  // namespace heartbeat_keys

// 设备心跳请求
struct DeviceHeartbeatReq {
  std::string device_sn;
  int32_t timestamp = 0;  // 客户端时间戳（秒）

  nlohmann::json ToJson() const {
    nlohmann::json json;
    json[heartbeat_keys::kDeviceSn] = device_sn;
    json[heartbeat_keys::kTimestamp] = timestamp;
    return json;
  }

  static DeviceHeartbeatReq FromJson(const nlohmann::json& json) {
    DeviceHeartbeatReq req;
    if (json.contains(heartbeat_keys::kDeviceSn)) {
      req.device_sn = json[heartbeat_keys::kDeviceSn].get<std::string>();
    }
    if (json.contains(heartbeat_keys::kTimestamp)) {
      req.timestamp = json[heartbeat_keys::kTimestamp].get<int64_t>();
    }
    return req;
  }
};

// 设备心跳响应
struct DeviceHeartbeatResp {
  int32_t server_time = 0;              // 服务器时间戳（秒）
  int32_t heartbeat_interval_sec = 60;  // 心跳间隔（秒），默认60秒
  bool need_config_sync = false;        // 是否需要配置同步

  nlohmann::json ToJson() const {
    nlohmann::json json;
    json[heartbeat_keys::kServerTime] = server_time;
    json[heartbeat_keys::kHeartbeatIntervalSec] = heartbeat_interval_sec;
    json[heartbeat_keys::kNeedConfigSync] = need_config_sync;
    return json;
  }

  static DeviceHeartbeatResp FromJson(const nlohmann::json& json) {
    DeviceHeartbeatResp resp;
    if (json.contains(heartbeat_keys::kServerTime)) {
      // 服务器可能返回字符串或数字格式的时间戳，需要兼容处理
      const auto& server_time_value = json[heartbeat_keys::kServerTime];
      if (server_time_value.is_string()) {
        resp.server_time = std::stoll(server_time_value.get<std::string>());
      } else if (server_time_value.is_number()) {
        resp.server_time = server_time_value.get<int64_t>();
      }
    }
    if (json.contains(heartbeat_keys::kHeartbeatIntervalSec)) {
      resp.heartbeat_interval_sec = json[heartbeat_keys::kHeartbeatIntervalSec].get<int32_t>();
    }
    if (json.contains(heartbeat_keys::kNeedConfigSync)) {
      resp.need_config_sync = json[heartbeat_keys::kNeedConfigSync].get<bool>();
    }
    return resp;
  }
};

}  // namespace entity
}  // namespace palm