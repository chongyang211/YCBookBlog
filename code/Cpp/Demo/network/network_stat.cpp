// Copyright © 1998 - 2024 Tencent. All Rights Reserved.

#include "network/network_stat.h"

#include "report/protocol/palm_report_item.h"

namespace palm {

std::chrono::system_clock::time_point NetworkStat::GetLastNetAvailableTime() { return last_net_available_time_; }

void NetworkStat::ReportTdiRequest(bool shortlink, uint32_t cmdid, std::string_view relative_url, int32_t error,
                                   std::string_view errmsg, int64_t timecost) {
  if (error == 0) {
    last_net_available_time_ = std::chrono::system_clock::now();
    return;
  }
  auto rpt = PalmReportItem::Create();
  rpt->action = "face.iotservice.api.request.fail";
  rpt->result_code = error;
  rpt->result_msg = errmsg;
  rpt->ext_key_1 = "request_type";
  rpt->ext_str_value_1 = cmdid ? std::to_string(cmdid) : relative_url;
  rpt->ext_key_2 = "translation_type";
  rpt->ext_str_value_2 = shortlink ? "HTTPS" : "ILINK";
  rpt->ext_number_key = "translation_time_cost";
  rpt->ext_number_value_1 = timecost;
  rpt.Commit();
}

static constexpr char kDeviceRequestMsgTypeReport[] = "COMM_REPORT";

void NetworkStat::ReportDeviceRequest(bool shortlink, std::string_view msg_type, int32_t error, std::string_view errmsg,
                                      int up_len, int down_len, int64_t timecost) {
  if (error == 0) {
    last_net_available_time_ = std::chrono::system_clock::now();
    return;
  }
  if (msg_type == kDeviceRequestMsgTypeReport) return;  // 上报类的请求不触发网络结果上报 (CRITICAL !!!)
  auto rpt = PalmReportItem::Create();
  rpt->action = "face.iotservice.api.request.fail";
  rpt->result_code = error;
  rpt->result_msg = errmsg;
  rpt->ext_key_1 = std::to_string((up_len + down_len));  // change total
  rpt->ext_str_value_1 = msg_type;
  rpt->ext_key_2 = "up:" + std::to_string(up_len) + " down:" + std::to_string(down_len);
  rpt->ext_str_value_2 = shortlink ? "HTTPS" : "ILINK";
  rpt->ext_number_key = "translation_time_cost";
  rpt->ext_number_value_1 = timecost;
  rpt.Commit();
}

std::chrono::system_clock::time_point NetworkStat::last_net_available_time_;

}  // namespace palm
