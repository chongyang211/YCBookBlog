// Copyright © 1998 - 2020 Tencent. All Rights Reserved.

#pragma once

#include <string>
#include <vector>
#include "nlohmann/json.hpp"

namespace palm {

// 升级后置动作枚举
enum class UpgradePostAction {
  kUnknown = 0,              // 未知
  kNone = 1,                 // 无动作
  kRestartApp = 2,           // 重启APP
  kRestartDevice = 3,        // 重启设备
  kRestartDeviceImmediately = 4  // 立即重启设备
};

namespace entity {

// 升级模式枚举
enum class UpgradeMode {
  NO_UPGRADE = 0,  // 无升级
  ADVICE = 1,      // 建议升级
  FORCE = 2,       // 强制升级
  ATONCE = 3       // 立即升级
};

// JSON key 常量定义
namespace upgrade_keys {
  // AppVersion 字段
  constexpr const char* kVersion = "version";
  constexpr const char* kApplicationId = "application_id";
  constexpr const char* kDetail = "detail";

  // BatchQueryUpgradeDetailReq 字段
  constexpr const char* kDeviceSn = "device_sn";
  constexpr const char* kAppVersions = "app_versions";

  // BatchQueryUpgradeDetailResp 字段
  constexpr const char* kDeviceUpgradeExecDetails = "device_upgrade_exec_details";

  // DeviceUpgradeExecDetail 字段
  constexpr const char* kDownloadUrl = "download_url";
  constexpr const char* kDownloadMac = "download_mac";
  constexpr const char* kUpgradeTaskUid = "upgrade_task_uid";
  constexpr const char* kUpgradeMode = "upgrade_mode";
  constexpr const char* kChangeLog = "change_log";
  constexpr const char* kInstallRequirements = "install_requirements";
  constexpr const char* kIncrementalUpgrade = "incremental_upgrade";

  // InstallRequirements 字段
  constexpr const char* kInstallStartTime = "install_start_time";
  constexpr const char* kIsAsyncInstall = "is_async_install";
  constexpr const char* kUpgradeFinishAction = "upgrade_finish_action";
  constexpr const char* kInstallWasteTime = "install_waste_time";
  constexpr const char* kMinSdkVersion = "min_sdk_version";

  // IncrementalUpgrade 字段
  constexpr const char* kForceIncrementalPackage = "force_incremental_package";
  constexpr const char* kStandardVersion = "standard_version";
}  // namespace upgrade_keys

// 应用版本信息
struct AppVersion {
  std::string version;
  std::string application_id;
  std::string detail;

  nlohmann::json ToJson() const {
    nlohmann::json json;
    json[upgrade_keys::kVersion] = version;
    json[upgrade_keys::kApplicationId] = application_id;
    json[upgrade_keys::kDetail] = detail;
    return json;
  }

  static AppVersion FromJson(const nlohmann::json& json) {
    AppVersion app;
    if (json.contains(upgrade_keys::kVersion)) {
      app.version = json[upgrade_keys::kVersion].get<std::string>();
    }
    if (json.contains(upgrade_keys::kApplicationId)) {
      app.application_id = json[upgrade_keys::kApplicationId].get<std::string>();
    }
    if (json.contains(upgrade_keys::kDetail)) {
      app.detail = json[upgrade_keys::kDetail].get<std::string>();
    }
    return app;
  }
};

// BatchQueryUpgradeDetail 请求
struct BatchQueryUpgradeDetailReq {
  std::string device_sn;
  std::vector<AppVersion> app_versions;

  nlohmann::json ToJson() const {
    nlohmann::json json;
    json[upgrade_keys::kDeviceSn] = device_sn;
    nlohmann::json app_versions_array = nlohmann::json::array();
    for (const auto& app : app_versions) {
      app_versions_array.push_back(app.ToJson());
    }
    json[upgrade_keys::kAppVersions] = app_versions_array;
    return json;
  }

  static BatchQueryUpgradeDetailReq FromJson(const nlohmann::json& json) {
    BatchQueryUpgradeDetailReq req;
    if (json.contains(upgrade_keys::kDeviceSn)) {
      req.device_sn = json[upgrade_keys::kDeviceSn].get<std::string>();
    }
    if (json.contains(upgrade_keys::kAppVersions) && json[upgrade_keys::kAppVersions].is_array()) {
      for (const auto& item : json[upgrade_keys::kAppVersions]) {
        req.app_versions.push_back(AppVersion::FromJson(item));
      }
    }
    return req;
  }
};

// 增量升级特性结构体
struct IncrementalUpgrade {
  bool force_incremental_package = false;  // 是否强制使用增量安装包
  std::string download_mac;                // 安装包mac校验值
  std::string download_url;                // 下载地址
  std::string standard_version;            // 基准版本

  nlohmann::json ToJson() const {
    nlohmann::json json;
    json[upgrade_keys::kForceIncrementalPackage] = force_incremental_package;
    json[upgrade_keys::kDownloadMac] = download_mac;
    json[upgrade_keys::kDownloadUrl] = download_url;
    json[upgrade_keys::kStandardVersion] = standard_version;
    return json;
  }

  static IncrementalUpgrade FromJson(const nlohmann::json& json) {
    IncrementalUpgrade upgrade;
    if (json.contains(upgrade_keys::kForceIncrementalPackage)) {
      upgrade.force_incremental_package = json[upgrade_keys::kForceIncrementalPackage].get<bool>();
    }
    if (json.contains(upgrade_keys::kDownloadMac)) {
      upgrade.download_mac = json[upgrade_keys::kDownloadMac].get<std::string>();
    }
    if (json.contains(upgrade_keys::kDownloadUrl)) {
      upgrade.download_url = json[upgrade_keys::kDownloadUrl].get<std::string>();
    }
    if (json.contains(upgrade_keys::kStandardVersion)) {
      upgrade.standard_version = json[upgrade_keys::kStandardVersion].get<std::string>();
    }
    return upgrade;
  }
};

// 升级明细的安装特性结构体
struct InstallRequirements {
  std::string install_start_time;                                   // 安装开始时间
  bool is_async_install = false;                                    // 是否异步安装
  UpgradePostAction upgrade_finish_action = UpgradePostAction::kUnknown;  // 升级后置动作
  std::string install_waste_time;                                   // 安装预估耗时
  std::string min_sdk_version;                                      // 最小sdk版本

  nlohmann::json ToJson() const {
    nlohmann::json json;
    json[upgrade_keys::kInstallStartTime] = install_start_time;
    json[upgrade_keys::kIsAsyncInstall] = is_async_install;
    json[upgrade_keys::kUpgradeFinishAction] = static_cast<int>(upgrade_finish_action);
    json[upgrade_keys::kInstallWasteTime] = install_waste_time;
    json[upgrade_keys::kMinSdkVersion] = min_sdk_version;
    return json;
  }

  static InstallRequirements FromJson(const nlohmann::json& json) {
    InstallRequirements req;
    if (json.contains(upgrade_keys::kInstallStartTime)) {
      auto& field = json[upgrade_keys::kInstallStartTime];
      req.install_start_time = field.is_string() ? field.get<std::string>() : std::to_string(field.get<uint64_t>());
    }
    if (json.contains(upgrade_keys::kIsAsyncInstall)) {
      req.is_async_install = json[upgrade_keys::kIsAsyncInstall].get<bool>();
    }
    if (json.contains(upgrade_keys::kUpgradeFinishAction)) {
      int action = json[upgrade_keys::kUpgradeFinishAction].get<int>();
      req.upgrade_finish_action = static_cast<UpgradePostAction>(action);
    }
    if (json.contains(upgrade_keys::kInstallWasteTime)) {
      auto& field = json[upgrade_keys::kInstallWasteTime];
      req.install_waste_time = field.is_string() ? field.get<std::string>() : std::to_string(field.get<uint32_t>());
    }
    if (json.contains(upgrade_keys::kMinSdkVersion)) {
      req.min_sdk_version = json[upgrade_keys::kMinSdkVersion].get<std::string>();
    }
    return req;
  }
};

// 升级详情（响应内部结构）
struct DeviceUpgradeExecDetail {
  std::string application_id;                  // 软件id
  std::string download_url;                    // 下载地址
  std::string download_mac;                    // 安装包mac校验值
  std::string version;                         // 升级包版本
  std::string upgrade_task_uid;                // 升级任务ID
  UpgradeMode upgrade_mode = UpgradeMode::NO_UPGRADE;  // 升级方式
  std::string change_log;                      // 安装包的change_log
  InstallRequirements install_requirements;    // 安装要求
  IncrementalUpgrade incremental_upgrade;      // 增量升级特性

  inline bool Match(const DeviceUpgradeExecDetail& rhs) const {
    return upgrade_task_uid == rhs.upgrade_task_uid && download_mac == rhs.download_mac;
  }

  nlohmann::json ToJson() const {
    nlohmann::json json;
    json[upgrade_keys::kApplicationId] = application_id;
    json[upgrade_keys::kDownloadUrl] = download_url;
    json[upgrade_keys::kDownloadMac] = download_mac;
    json[upgrade_keys::kVersion] = version;
    json[upgrade_keys::kUpgradeTaskUid] = upgrade_task_uid;
    json[upgrade_keys::kUpgradeMode] = static_cast<int>(upgrade_mode);
    json[upgrade_keys::kChangeLog] = change_log;
    json[upgrade_keys::kInstallRequirements] = install_requirements.ToJson();
    json[upgrade_keys::kIncrementalUpgrade] = incremental_upgrade.ToJson();
    return json;
  }

  static DeviceUpgradeExecDetail FromJson(const nlohmann::json& json) {
    DeviceUpgradeExecDetail detail;
    if (json.contains(upgrade_keys::kApplicationId)) {
      detail.application_id = json[upgrade_keys::kApplicationId].get<std::string>();
    }
    if (json.contains(upgrade_keys::kDownloadUrl)) {
      detail.download_url = json[upgrade_keys::kDownloadUrl].get<std::string>();
    }
    if (json.contains(upgrade_keys::kDownloadMac)) {
      detail.download_mac = json[upgrade_keys::kDownloadMac].get<std::string>();
    }
    if (json.contains(upgrade_keys::kVersion)) {
      detail.version = json[upgrade_keys::kVersion].get<std::string>();
    }
    if (json.contains(upgrade_keys::kUpgradeTaskUid)) {
      detail.upgrade_task_uid = json[upgrade_keys::kUpgradeTaskUid].get<std::string>();
    }
    if (json.contains(upgrade_keys::kUpgradeMode)) {
      int mode = json[upgrade_keys::kUpgradeMode].get<int>();
      detail.upgrade_mode = static_cast<UpgradeMode>(mode);
    }
    if (json.contains(upgrade_keys::kChangeLog)) {
      detail.change_log = json[upgrade_keys::kChangeLog].get<std::string>();
    }
    if (json.contains(upgrade_keys::kInstallRequirements)) {
      detail.install_requirements = InstallRequirements::FromJson(json[upgrade_keys::kInstallRequirements]);
    }
    if (json.contains(upgrade_keys::kIncrementalUpgrade)) {
      detail.incremental_upgrade = IncrementalUpgrade::FromJson(json[upgrade_keys::kIncrementalUpgrade]);
    }
    return detail;
  }
};

// BatchQueryUpgradeDetail 响应
struct BatchQueryUpgradeDetailResp {
  std::vector<DeviceUpgradeExecDetail> device_upgrade_exec_details;

  nlohmann::json ToJson() const {
    nlohmann::json json;
    nlohmann::json details_array = nlohmann::json::array();
    for (const auto& detail : device_upgrade_exec_details) {
      details_array.push_back(detail.ToJson());
    }
    json[upgrade_keys::kDeviceUpgradeExecDetails] = details_array;
    return json;
  }

  static BatchQueryUpgradeDetailResp FromJson(const nlohmann::json& json) {
    BatchQueryUpgradeDetailResp resp;
    if (json.contains(upgrade_keys::kDeviceUpgradeExecDetails) &&
        json[upgrade_keys::kDeviceUpgradeExecDetails].is_array()) {
      for (const auto& item : json[upgrade_keys::kDeviceUpgradeExecDetails]) {
        resp.device_upgrade_exec_details.push_back(
            DeviceUpgradeExecDetail::FromJson(item));
      }
    }
    return resp;
  }
};

}  // namespace entity
}  // namespace palm
