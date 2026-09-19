// Copyright © 1998 - 2020 Tencent. All Rights Reserved.

#pragma once

#include <cstdint>
#include <string>

#include "nlohmann/json.hpp"

namespace palm {
namespace entity {

// JSON key 常量定义
namespace instruction_presign_keys {
  // GenerateInstructionPresignInfoReq 字段
  constexpr const char* kDeviceSn = "device_sn";
  constexpr const char* kPrefixPath = "prefix_path";
  constexpr const char* kFileExtension = "file_extension";
  constexpr const char* kPutExpiredDuration = "put_expired_duration";
  constexpr const char* kGetExpiredDuration = "get_expired_duration";

  // GenerateInstructionPresignInfoResp 字段
  constexpr const char* kUploadUrl = "upload_url";
  constexpr const char* kDownloadUrl = "download_url";
  constexpr const char* kContentType = "content_type";
}  // namespace instruction_presign_keys

// 获取预上传 URL 请求
struct GenerateInstructionPresignInfoReq {
  std::string device_sn;              // 设备 sn
  std::string prefix_path;            // 路径前缀
  std::string file_extension;         // 上传文件后缀
  std::string put_expired_duration;   // 上传过期时长（秒），可选默认一天
  std::string get_expired_duration;   // 下载过期时长（秒），可选默认一个月

  nlohmann::json ToJson() const;
  static GenerateInstructionPresignInfoReq FromJson(const nlohmann::json& json);
};

// 获取预上传 URL 响应
struct GenerateInstructionPresignInfoResp {
  std::string upload_url;    // 上传 url
  std::string download_url;  // 下载 url
  std::string content_type;  // 上传时使用的 Content-Type

  nlohmann::json ToJson() const;
  static GenerateInstructionPresignInfoResp FromJson(
      const nlohmann::json& json);
};

}  // namespace entity
}  // namespace palm
