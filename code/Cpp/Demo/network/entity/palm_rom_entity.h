// Copyright © 1998 - 2020 Tencent. All Rights Reserved.

#pragma once

#include <string>
#include "nlohmann/json.hpp"

namespace palm {
namespace entity {

// JSON key 常量定义
namespace palm_rom_keys {
  // QueryPalmRomRegReq 字段
  constexpr const char* kRomModel = "rom_model";

  // QueryPalmRomRegResp 字段
  constexpr const char* kRomId = "rom_id";
  constexpr const char* kPlatApplicationId = "plat_application_id";
}  // namespace palm_rom_keys

// QueryPalmRomReg 请求
struct QueryPalmRomRegReq {
  std::string rom_model;

  nlohmann::json ToJson() const {
    nlohmann::json json;
    json[palm_rom_keys::kRomModel] = rom_model;
    return json;
  }

  static QueryPalmRomRegReq FromJson(const nlohmann::json& json) {
    QueryPalmRomRegReq req;
    if (json.contains(palm_rom_keys::kRomModel)) {
      req.rom_model = json[palm_rom_keys::kRomModel].get<std::string>();
    }
    return req;
  }
};

// QueryPalmRomReg 响应
struct QueryPalmRomRegResp {
  uint32_t rom_id = 0;
  std::string plat_application_id;

  nlohmann::json ToJson() const {
    nlohmann::json json;
    json[palm_rom_keys::kRomId] = rom_id;
    json[palm_rom_keys::kPlatApplicationId] = plat_application_id;
    return json;
  }

  static QueryPalmRomRegResp FromJson(const nlohmann::json& json) {
    QueryPalmRomRegResp resp;
    if (json.contains(palm_rom_keys::kRomId)) {
      resp.rom_id = json[palm_rom_keys::kRomId].get<uint32_t>();
    }
    if (json.contains(palm_rom_keys::kPlatApplicationId)) {
      resp.plat_application_id = json[palm_rom_keys::kPlatApplicationId].get<std::string>();
    }
    return resp;
  }
};

}  // namespace entity
}  // namespace palm
