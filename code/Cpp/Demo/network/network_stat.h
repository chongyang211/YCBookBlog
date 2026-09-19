// Copyright © 1998 - 2024 Tencent. All Rights Reserved.

#pragma once

#include "comm/comm.h"

namespace palm {
class NetworkStat {
 public:
  static std::chrono::system_clock::time_point GetLastNetAvailableTime();

 public:
  static void ReportTdiRequest(bool shortlink, uint32_t cmdid, std::string_view relative_url, int32_t error,
                               std::string_view errmsg, int64_t timecost);

  static void ReportDeviceRequest(bool shortlink, std::string_view msg_type, int32_t error, std::string_view errmsg,
                                  int up_len, int down_len, int64_t timecost);

 private:
  static std::chrono::system_clock::time_point last_net_available_time_;
};
}  // namespace palm
