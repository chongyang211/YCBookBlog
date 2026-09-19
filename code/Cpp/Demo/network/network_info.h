// Copyright © 1998 - 2023 Tencent. All Rights Reserved.

#pragma once

#include "comm/comm.h"

namespace palm {

enum class NetworkType {
  kNone = 10000,
  kCellular = 10001,
  kWifi = 10002,
  kEthernet = 10003,
  kOther = 10004,
};

// Convert NetworkType to DBus network type string (uses SystemInterface constants)
const char* NetworkTypeToString(NetworkType type);

// Convert DBus network type string to NetworkType enum
NetworkType StringToNetworkType(const char* type_str);

// Network connection state enum
enum class NetworkConnectState {
  kDisconnected = 0,  // Disconnected
  kConnecting = 1,    // Connecting
  kConnected = 2,     // Connected
  kUnknown = 99,      // Unknown state
};

// Convert NetworkConnectState to string
const char* NetworkConnectStateToString(NetworkConnectState state);

class NetworkInfo {
 private:
  NetworkInfo() = default;

 public:
  static NetworkInfo *Instance();

  // Get current network type
  NetworkType GetNetworkType() const { return net_type_; }

  // Get current connection state
  NetworkConnectState GetConnectState() const { return connect_state_; }

  // Check if network is connected
  bool IsConnected() const { return connect_state_ == NetworkConnectState::kConnected; }

 protected:
  void UpdateNetworkType(NetworkType type);
  void UpdateConnectState(NetworkConnectState state);
  friend class NetworkMonitorService;

 private:
  NetworkType net_type_ = NetworkType::kNone;
  NetworkConnectState connect_state_ = NetworkConnectState::kUnknown;
};
}  // namespace palm
