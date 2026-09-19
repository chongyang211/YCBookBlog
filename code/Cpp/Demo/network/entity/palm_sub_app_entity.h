// Copyright © 1998 - 2020 Tencent. All Rights Reserved.

#pragma once

#include <string>
#include "nlohmann/json.hpp"

namespace palm {
namespace entity {

// JSON key 常量定义
namespace palm_sub_app_keys {
  // QueryPalmSubAppRegReq 字段
  constexpr const char* kAppId = "app_id";
  constexpr const char* kPackageName = "package_name";
  constexpr const char* kAppSignature = "app_signature";
  constexpr const char* kSubAppScene = "sub_app_scene";

  // QueryPalmSubAppRegResp 字段
  constexpr const char* kSubAppId = "sub_app_id";
  constexpr const char* kPlatApplicationId = "plat_application_id";
}  // namespace palm_sub_app_keys

// QueryPalmSubAppReg 请求
struct QueryPalmSubAppRegReq {
  std::string app_id;
  std::string package_name;
  std::string app_signature;
  std::string sub_app_scene;

  nlohmann::json ToJson() const {
    nlohmann::json json;
    json[palm_sub_app_keys::kAppId] = app_id;
    json[palm_sub_app_keys::kPackageName] = package_name;
    json[palm_sub_app_keys::kAppSignature] = app_signature;
    json[palm_sub_app_keys::kSubAppScene] = sub_app_scene;
    return json;
  }

  static QueryPalmSubAppRegReq FromJson(const nlohmann::json& json) {
    QueryPalmSubAppRegReq req;
    if (json.contains(palm_sub_app_keys::kAppId)) {
      req.app_id = json[palm_sub_app_keys::kAppId].get<std::string>();
    }
    if (json.contains(palm_sub_app_keys::kPackageName)) {
      req.package_name = json[palm_sub_app_keys::kPackageName].get<std::string>();
    }
    if (json.contains(palm_sub_app_keys::kAppSignature)) {
      req.app_signature = json[palm_sub_app_keys::kAppSignature].get<std::string>();
    }
    if (json.contains(palm_sub_app_keys::kSubAppScene)) {
      req.sub_app_scene = json[palm_sub_app_keys::kSubAppScene].get<std::string>();
    }
    return req;
  }
};

// QueryPalmSubAppReg 响应
struct QueryPalmSubAppRegResp {
  uint32_t sub_app_id = 0;
  std::string plat_application_id;

  nlohmann::json ToJson() const {
    nlohmann::json json;
    json[palm_sub_app_keys::kSubAppId] = sub_app_id;
    json[palm_sub_app_keys::kPlatApplicationId] = plat_application_id;
    return json;
  }

  static QueryPalmSubAppRegResp FromJson(const nlohmann::json& json) {
    QueryPalmSubAppRegResp resp;
    if (json.contains(palm_sub_app_keys::kSubAppId)) {
      resp.sub_app_id = json[palm_sub_app_keys::kSubAppId].get<uint32_t>();
    }
    if (json.contains(palm_sub_app_keys::kPlatApplicationId)) {
      resp.plat_application_id = json[palm_sub_app_keys::kPlatApplicationId].get<std::string>();
    }
    return resp;
  }
};

}  // namespace entity
}  // namespace palm
