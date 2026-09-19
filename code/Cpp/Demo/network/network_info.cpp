// Copyright © 1998 - 2023 Tencent. All Rights Reserved.

#include "network/network_info.h"

#include <cstring>

#include "system/system_interface.h"

namespace palm {

const char* NetworkTypeToString(NetworkType type) {
  switch (type) {
    case NetworkType::kWifi:
      return SystemInterface::kNetworkTypeWifi;
    case NetworkType::kEthernet:
      return SystemInterface::kNetworkTypeEthernet;
    case NetworkType::kCellular:
      return SystemInterface::kNetworkTypeCell;
    case NetworkType::kNone:
    case NetworkType::kOther:
    default:
      return "";
  }
}

NetworkType StringToNetworkType(const char* type_str) {
  if (!type_str || type_str[0] == '\0') {
    return NetworkType::kNone;
  }

  if (std::strcmp(type_str, SystemInterface::kNetworkTypeWifi) == 0) {
    return NetworkType::kWifi;
  } else if (std::strcmp(type_str, SystemInterface::kNetworkTypeEthernet) == 0) {
    return NetworkType::kEthernet;
  } else if (std::strcmp(type_str, SystemInterface::kNetworkTypeCell) == 0) {
    return NetworkType::kCellular;
  }

  return NetworkType::kOther;
}

const char* NetworkConnectStateToString(NetworkConnectState state) {
  switch (state) {
    case NetworkConnectState::kDisconnected:
      return "disconnected";
    case NetworkConnectState::kConnecting:
      return "connecting";
    case NetworkConnectState::kConnected:
      return "connected";
    case NetworkConnectState::kUnknown:
    default:
      return "unknown";
  }
}

NetworkInfo *NetworkInfo::Instance() {
  static NetworkInfo inst;
  return &inst;
}

void NetworkInfo::UpdateNetworkType(NetworkType type) { net_type_ = type; }

void NetworkInfo::UpdateConnectState(NetworkConnectState state) { connect_state_ = state; }

}  // namespace palm
