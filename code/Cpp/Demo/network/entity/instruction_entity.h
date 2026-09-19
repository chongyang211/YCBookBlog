// Copyright © 1998 - 2020 Tencent. All Rights Reserved.

#pragma once

#include <string>
#include <vector>
#include "nlohmann/json.hpp"

namespace palm {
namespace entity {

// JSON key 常量定义
namespace instruction_keys {
  // 通用字段
  constexpr const char* kInstructions = "instructions";
  constexpr const char* kState = "state";
  constexpr const char* kData = "data";

  // InstructionForDevice 字段
  constexpr const char* kId = "id";
  constexpr const char* kName = "name";
  constexpr const char* kTarget = "target";
  constexpr const char* kBusinessParams = "business_params";
  constexpr const char* kCreateTime = "create_time";
  constexpr const char* kExecuteTime = "execute_time";
  constexpr const char* kExpireTime = "expire_time";
  constexpr const char* kSnapshot = "snapshot";
  constexpr const char* kSignature = "signature";

  // InstructionSnapshot 字段
  constexpr const char* kSeq = "seq";
  constexpr const char* kBusinessResult = "business_result";
  constexpr const char* kBusinessInfo = "business_info";
  constexpr const char* kErrorCode = "error_code";
  constexpr const char* kErrorMsg = "error_msg";

  // UpdateInstructionExecStateReq 字段
  constexpr const char* kDeviceSn = "device_sn";
  constexpr const char* kInstructionId = "instruction_id";
}  // namespace instruction_keys

// 指令业务自定义信息
struct InstructionBusinessInfo {
  uint32_t state = 0;  // 业务自定义指令状态
  std::string data;    // 业务自定义指令额外信息

  nlohmann::json ToJson() const {
    nlohmann::json json;
    json[instruction_keys::kState] = state;
    json[instruction_keys::kData] = data;
    return json;
  }

  static InstructionBusinessInfo FromJson(const nlohmann::json& json) {
    InstructionBusinessInfo info;
    if (json.contains(instruction_keys::kState)) {
      info.state = json[instruction_keys::kState].get<uint32_t>();
    }
    if (json.contains(instruction_keys::kData)) {
      info.data = json[instruction_keys::kData].get<std::string>();
    }
    return info;
  }
};

// 设备侧所需的指令快照信息
struct InstructionSnapshot {
  uint32_t seq = 0;                           // 序列号
  std::string state;                          // 状态（字符串）
  std::string business_result;                // 执行结果
  InstructionBusinessInfo business_info;      // 业务自定义信息
  std::string error_code;                     // 错误码（字符串）
  std::string error_msg;                      // 错误信息

  nlohmann::json ToJson() const {
    nlohmann::json json;
    json[instruction_keys::kSeq] = seq;
    json[instruction_keys::kState] = state;
    json[instruction_keys::kBusinessResult] = business_result;
    json[instruction_keys::kBusinessInfo] = business_info.ToJson();
    json[instruction_keys::kErrorCode] = error_code;
    json[instruction_keys::kErrorMsg] = error_msg;
    return json;
  }

  static InstructionSnapshot FromJson(const nlohmann::json& json) {
    InstructionSnapshot snapshot;
    if (json.contains(instruction_keys::kSeq)) {
      snapshot.seq = json[instruction_keys::kSeq].get<uint32_t>();
    }
    if (json.contains(instruction_keys::kState)) {
      snapshot.state = json[instruction_keys::kState].get<std::string>();
    }
    if (json.contains(instruction_keys::kBusinessResult)) {
      snapshot.business_result = json[instruction_keys::kBusinessResult].get<std::string>();
    }
    if (json.contains(instruction_keys::kBusinessInfo)) {
      snapshot.business_info = InstructionBusinessInfo::FromJson(json[instruction_keys::kBusinessInfo]);
    }
    if (json.contains(instruction_keys::kErrorCode)) {
      snapshot.error_code = json[instruction_keys::kErrorCode].get<std::string>();
    }
    if (json.contains(instruction_keys::kErrorMsg)) {
      snapshot.error_msg = json[instruction_keys::kErrorMsg].get<std::string>();
    }
    return snapshot;
  }
};

// 指令信息（设备侧）
struct Instruction {
  std::string id;                   // 指令编号
  std::string name;                 // 指令名称
  std::string target;               // 目标 APP
  std::string business_params;      // 指令参数
  std::string create_time;          // 创建时间（字符串）
  std::string execute_time;         // 执行时间（字符串）
  std::string expire_time;          // 过期时间（字符串）
  InstructionSnapshot snapshot;     // 快照
  std::string signature;            // 签名

  nlohmann::json ToJson() const {
    nlohmann::json json;
    json[instruction_keys::kId] = id;
    json[instruction_keys::kName] = name;
    json[instruction_keys::kTarget] = target;
    json[instruction_keys::kBusinessParams] = business_params;
    json[instruction_keys::kCreateTime] = create_time;
    json[instruction_keys::kExecuteTime] = execute_time;
    json[instruction_keys::kExpireTime] = expire_time;
    json[instruction_keys::kSnapshot] = snapshot.ToJson();
    json[instruction_keys::kSignature] = signature;
    return json;
  }

  static Instruction FromJson(const nlohmann::json& json) {
    Instruction instruction;
    if (json.contains(instruction_keys::kId)) {
      instruction.id = json[instruction_keys::kId].get<std::string>();
    }
    if (json.contains(instruction_keys::kName)) {
      instruction.name = json[instruction_keys::kName].get<std::string>();
    }
    if (json.contains(instruction_keys::kTarget)) {
      instruction.target = json[instruction_keys::kTarget].get<std::string>();
    }
    if (json.contains(instruction_keys::kBusinessParams)) {
      instruction.business_params = json[instruction_keys::kBusinessParams].get<std::string>();
    }
    if (json.contains(instruction_keys::kCreateTime)) {
      instruction.create_time = json[instruction_keys::kCreateTime].get<std::string>();
    }
    if (json.contains(instruction_keys::kExecuteTime)) {
      instruction.execute_time = json[instruction_keys::kExecuteTime].get<std::string>();
    }
    if (json.contains(instruction_keys::kExpireTime)) {
      instruction.expire_time = json[instruction_keys::kExpireTime].get<std::string>();
    }
    if (json.contains(instruction_keys::kSnapshot)) {
      instruction.snapshot = InstructionSnapshot::FromJson(json[instruction_keys::kSnapshot]);
    }
    if (json.contains(instruction_keys::kSignature)) {
      instruction.signature = json[instruction_keys::kSignature].get<std::string>();
    }
    return instruction;
  }
};

// QueryInstructionExecuteList 响应
struct QueryInstructionExecuteListResp {
  std::vector<Instruction> instructions;

  nlohmann::json ToJson() const {
    nlohmann::json json;
    nlohmann::json instructions_array = nlohmann::json::array();
    for (const auto& inst : instructions) {
      instructions_array.push_back(inst.ToJson());
    }
    json[instruction_keys::kInstructions] = instructions_array;
    return json;
  }

  static QueryInstructionExecuteListResp FromJson(const nlohmann::json& json) {
    QueryInstructionExecuteListResp resp;
    if (json.contains(instruction_keys::kInstructions) && json[instruction_keys::kInstructions].is_array()) {
      for (const auto& item : json[instruction_keys::kInstructions]) {
        resp.instructions.push_back(Instruction::FromJson(item));
      }
    }
    return resp;
  }
};

// 业务信息 (用于更新指令执行状态)
struct BusinessInfo {
  uint32_t state = 0;  // 业务自定义指令状态
  std::string data;    // 业务自定义指令额外信息

  nlohmann::json ToJson() const {
    nlohmann::json json;
    json[instruction_keys::kState] = state;
    json[instruction_keys::kData] = data;
    return json;
  }

  static BusinessInfo FromJson(const nlohmann::json& json) {
    BusinessInfo info;
    if (json.contains(instruction_keys::kState)) {
      info.state = json[instruction_keys::kState].get<uint32_t>();
    }
    if (json.contains(instruction_keys::kData)) {
      info.data = json[instruction_keys::kData].get<std::string>();
    }
    return info;
  }
};

// UpdateInstructionExecState 请求
struct UpdateInstructionExecStateReq {
  std::string device_sn;
  std::string instruction_id;
  uint32_t state = 0;
  std::string business_result;
  std::string error_code;  // 错误码（字符串）
  std::string error_msg;
  uint32_t seq = 0;
  BusinessInfo business_info;

  nlohmann::json ToJson() const {
    nlohmann::json json;
    json[instruction_keys::kDeviceSn] = device_sn;
    json[instruction_keys::kInstructionId] = instruction_id;
    json[instruction_keys::kState] = state;
    json[instruction_keys::kBusinessResult] = business_result;
    json[instruction_keys::kErrorCode] = error_code;
    json[instruction_keys::kErrorMsg] = error_msg;
    json[instruction_keys::kSeq] = seq;
    json[instruction_keys::kBusinessInfo] = business_info.ToJson();
    return json;
  }

  static UpdateInstructionExecStateReq FromJson(const nlohmann::json& json) {
    UpdateInstructionExecStateReq req;
    if (json.contains(instruction_keys::kDeviceSn)) {
      req.device_sn = json[instruction_keys::kDeviceSn].get<std::string>();
    }
    if (json.contains(instruction_keys::kInstructionId)) {
      req.instruction_id = json[instruction_keys::kInstructionId].get<std::string>();
    }
    if (json.contains(instruction_keys::kState)) {
      req.state = json[instruction_keys::kState].get<uint32_t>();
    }
    if (json.contains(instruction_keys::kBusinessResult)) {
      req.business_result = json[instruction_keys::kBusinessResult].get<std::string>();
    }
    if (json.contains(instruction_keys::kErrorCode)) {
      req.error_code = json[instruction_keys::kErrorCode].get<std::string>();
    }
    if (json.contains(instruction_keys::kErrorMsg)) {
      req.error_msg = json[instruction_keys::kErrorMsg].get<std::string>();
    }
    if (json.contains(instruction_keys::kSeq)) {
      req.seq = json[instruction_keys::kSeq].get<uint32_t>();
    }
    if (json.contains(instruction_keys::kBusinessInfo)) {
      req.business_info = BusinessInfo::FromJson(json[instruction_keys::kBusinessInfo]);
    }
    return req;
  }
};

// UpdateInstructionExecState 响应（空响应）
struct UpdateInstructionExecStateResp {
  nlohmann::json ToJson() const {
    return nlohmann::json::object();
  }

  static UpdateInstructionExecStateResp FromJson(const nlohmann::json& json) {
    return UpdateInstructionExecStateResp{};
  }
};

}  // namespace entity
}  // namespace palm
