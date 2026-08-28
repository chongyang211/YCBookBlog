//
// Created by 杨充 on 2026/5/28.
//

#ifndef RESERVATION_H
#define RESERVATION_H
#include <string>

enum class ResStatus {Pending, Approved, Rejected, Cancelled };

class Reservation {
public:
    int         resId;          // 预约 ID（自增）
    std::string studentId;      // 哪个学生（外键）
    int         computerId;     // 哪个机房（外键）
    std::string date;           // 日期 YYYY-MM-DD
    ResStatus   status = ResStatus::Pending;

    Reservation() = default;
    Reservation(int rid, const std::string& sid, int cid, const std::string& d)
        :resId(rid), studentId(sid), computerId(cid), date(d){}
    std::string statusText() const {
        switch (status) {
            case ResStatus::Pending:   return "待审核";
            case ResStatus::Approved:  return "已批准";
            case ResStatus::Rejected:  return "已拒绝";
            case ResStatus::Cancelled: return "已取消";
        }
        return "未知";
    }
};



#endif //RESERVATION_H
