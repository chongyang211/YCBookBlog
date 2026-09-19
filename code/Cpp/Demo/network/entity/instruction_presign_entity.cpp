// Copyright © 1998 - 2020 Tencent. All Rights Reserved.

#include "network/entity/instruction_presign_entity.h"

namespace palm {
namespace entity {

// ========== GenerateInstructionPresignInfoReq ==========

nlohmann::json GenerateInstructionPresignInfoReq::ToJson() const {
  nlohmann::json json;
  json[instruction_presign_keys::kDeviceSn] = device_sn;
  json[instruction_presign_keys::kPrefixPath] = prefix_path;
  json[instruction_presign_keys::kFileExtension] = file_extension;
  if (!put_expired_duration.empty()) {
    json[instruction_presign_keys::kPutExpiredDuration] = put_expired_duration;
  }
  if (!get_expired_duration.empty()) {
    json[instruction_presign_keys::kGetExpiredDuration] = get_expired_duration;
  }
  return json;
}

GenerateInstructionPresignInfoReq
GenerateInstructionPresignInfoReq::FromJson(const nlohmann::json& json) {
  GenerateInstructionPresignInfoReq req;
  if (json.contains(instruction_presign_keys::kDeviceSn)) {
    req.device_sn = json[instruction_presign_keys::kDeviceSn].get<std::string>();
  }
  if (json.contains(instruction_presign_keys::kPrefixPath)) {
    req.prefix_path = json[instruction_presign_keys::kPrefixPath].get<std::string>();
  }
  if (json.contains(instruction_presign_keys::kFileExtension)) {
    req.file_extension = json[instruction_presign_keys::kFileExtension].get<std::string>();
  }
  if (json.contains(instruction_presign_keys::kPutExpiredDuration)) {
    req.put_expired_duration = json[instruction_presign_keys::kPutExpiredDuration].get<std::string>();
  }
  if (json.contains(instruction_presign_keys::kGetExpiredDuration)) {
    req.get_expired_duration = json[instruction_presign_keys::kGetExpiredDuration].get<std::string>();
  }
  return req;
}

// ========== GenerateInstructionPresignInfoResp ==========

nlohmann::json GenerateInstructionPresignInfoResp::ToJson() const {
  nlohmann::json json;
  json[instruction_presign_keys::kUploadUrl] = upload_url;
  json[instruction_presign_keys::kDownloadUrl] = download_url;
  if (!content_type.empty()) {
    json[instruction_presign_keys::kContentType] = content_type;
  }
  return json;
}

GenerateInstructionPresignInfoResp
GenerateInstructionPresignInfoResp::FromJson(const nlohmann::json& json) {
  GenerateInstructionPresignInfoResp resp;
  if (json.contains(instruction_presign_keys::kUploadUrl)) {
    resp.upload_url = json[instruction_presign_keys::kUploadUrl].get<std::string>();
  }
  if (json.contains(instruction_presign_keys::kDownloadUrl)) {
    resp.download_url = json[instruction_presign_keys::kDownloadUrl].get<std::string>();
  }
  if (json.contains(instruction_presign_keys::kContentType)) {
    resp.content_type = json[instruction_presign_keys::kContentType].get<std::string>();
  }
  return resp;
}

}  // namespace entity
}  // namespace palm
