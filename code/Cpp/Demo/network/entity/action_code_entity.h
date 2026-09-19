// Copyright © 1998 - 2024 Tencent. All Rights Reserved.

#pragma once

#include <string>
#include "nlohmann/json.hpp"

namespace palm {
namespace entity {

namespace action_code_keys {
  // FetchCommonKey response
  constexpr const char* kPublicKey = "public_key";

  // FetchActionCodeDetail request
  constexpr const char* kVerifyCode = "verify_code";

  // FetchActionCodeDetail response
  constexpr const char* kActionCodeStr = "action_code_str";
}  // namespace action_code_keys

// GetDeviceCommonKey response
struct FetchCommonKeyResp {
  std::string public_key;

  nlohmann::json ToJson() const {
    nlohmann::json json;
    json[action_code_keys::kPublicKey] = public_key;
    return json;
  }

  static FetchCommonKeyResp FromJson(const nlohmann::json& json) {
    FetchCommonKeyResp resp;
    if (json.contains(action_code_keys::kPublicKey)) {
      resp.public_key = json[action_code_keys::kPublicKey].get<std::string>();
    }
    return resp;
  }
};

// GetActionCodeDetail request
struct FetchActionCodeDetailReq {
  std::string verify_code;

  nlohmann::json ToJson() const {
    nlohmann::json json;
    json[action_code_keys::kVerifyCode] = verify_code;
    return json;
  }
};

// GetActionCodeDetail response
struct FetchActionCodeDetailResp {
  std::string action_code_str;

  nlohmann::json ToJson() const {
    nlohmann::json json;
    json[action_code_keys::kActionCodeStr] = action_code_str;
    return json;
  }

  static FetchActionCodeDetailResp FromJson(const nlohmann::json& json) {
    FetchActionCodeDetailResp resp;
    if (json.contains(action_code_keys::kActionCodeStr)) {
      resp.action_code_str = json[action_code_keys::kActionCodeStr].get<std::string>();
    }
    return resp;
  }
};

}  // namespace entity
}  // namespace palm
