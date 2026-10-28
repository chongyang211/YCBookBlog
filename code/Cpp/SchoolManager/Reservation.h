//
// Created by 杨充 on 2026/5/28.
//
// ============================================================================
// 【文件定位】Reservation.h —— 预约单实体类（学生与机房之间的关联记录）
// ----------------------------------------------------------------------------
// 【设计思路】
//   1. 这是经典数据库思维的"多对多关联表"：学生(id) ←→ 机房(id)，
//      预约单记录"谁、何时、订了哪个机房、当前状态"。
//   2. 用"状态机"管理生命周期：Pending →(审核)→ Approved / Rejected，
//      Pending →(学生主动)→ Cancelled。所有状态转移集中在 CampusSystem，
//      保证不出现非法跳转（如已批准的预约不能取消）。
//   3. 状态用 enum class 强类型枚举表示，而非 int 魔法数字。
// ============================================================================

#ifndef RESERVATION_H // 头文件保护宏：防止重复包含
#define RESERVATION_H
#include <string>     // std::string 成员依赖

// 【技术原理】enum class（C++11 强类型枚举）vs 传统 enum：
//   - 枚举值带作用域：必须写 ResStatus::Pending，不能裸用 Pending，
//     避免不同枚举间的命名冲突（比如另一个 RoomStatus::Pending）；
//   - 不会隐式转换为 int，类型安全更强（不能拿状态直接当数组下标误用）；
//   - 底层类型默认 int，占 4 字节，成员默认初始化时可见下方的类内初始化。
enum class ResStatus {Pending, Approved, Rejected, Cancelled };
// 状态语义：Pending=待审核  Approved=教师已批准  Rejected=教师已拒绝  Cancelled=学生已取消

class Reservation {
public:
    int         resId;          // 预约 ID（自增）
    // 【技术原理】由 CampusSystem::nextResId 全局自增分配，
    //             保证每张预约单编号唯一，取消/审核时靠它精确定位。
    std::string studentId;      // 哪个学生（外键）
    // 【技术原理】"外键"思想：不嵌套整个 Student 对象（避免数据冗余与循环依赖），
    //             只存账号 id，需要时去 users map 里查 —— 与数据库外键等价。
    int         computerId;     // 哪个机房（外键）
    std::string date;           // 日期 YYYY-MM-DD
    // 【技术原理】类内初始化（C++11 NSDMI）：
    //             "= ResStatus::Pending" 让所有构造路径（包括默认构造）
    //             的 status 都有确定的初始值，避免未初始化的枚举产生未定义行为。
    ResStatus   status = ResStatus::Pending;

    // 【技术原理】= default 补回默认构造：定义了带参构造后编译器不再隐式生成无参版本，
    //             容器（vector 扩容、值初始化）需要默认构造可用。
    Reservation() = default;
    // 带参构造：初始化列表按声明顺序依次初始化四个成员；
    // status 不在参数中 —— 新预约单一律默认"待审核"，符合业务直觉。
    Reservation(int rid, const std::string& sid, int cid, const std::string& d)
        :resId(rid), studentId(sid), computerId(cid), date(d){}
    // 状态 → 中文文案的映射，集中在一处（避免 UI 代码到处写 switch 重复翻译）
    // 【技术原理】switch 覆盖所有枚举值；结尾 return "未知" 兜底，
    //             保证控制流一定有返回值（否则编译器告警"控制流到达非 void 函数末尾"）。
    std::string statusText() const {
        switch (status) {
            case ResStatus::Pending:   return "待审核";
            case ResStatus::Approved:  return "已批准";
            case ResStatus::Rejected:  return "已拒绝";
            case ResStatus::Cancelled: return "已取消";
        }
        return "未知";
    }

    // ============ CSV 序列化（阶段 ⑧：reservations.csv 的行格式）============
    // 行格式与文档 §9.2 对齐：resId,studentId,roomId,date,status
    // status 存英文枚举名（Pending/Approved/...）而非中文——CSV 是给机器读的，
    // 中文文案（statusText）是给界面看的，两者职责分离。

    // 枚举 → CSV 字段文本（static：不依赖任何实例，反序列化也要用）
    static std::string statusToText(ResStatus s) {
        switch (s) {
            case ResStatus::Pending:   return "Pending";
            case ResStatus::Approved:  return "Approved";
            case ResStatus::Rejected:  return "Rejected";
            case ResStatus::Cancelled: return "Cancelled";
        }
        return "Pending";   // 兜底（理论上到不了这里）
    }
    // CSV 字段文本 → 枚举（未知文本按 Pending 兜底，与"新单默认待审"的业务直觉一致）
    static ResStatus statusFromText(const std::string& t) {
        if (t == "Approved")  return ResStatus::Approved;
        if (t == "Rejected")  return ResStatus::Rejected;
        if (t == "Cancelled") return ResStatus::Cancelled;
        return ResStatus::Pending;
    }

    // 序列化为 CSV 一行（对象 → 字符串）
    std::string toCsv() const {
        return std::to_string(resId) + "," + studentId + "," +
               std::to_string(computerId) + "," + date + "," + statusToText(status);
    }

    // 反序列化（字符串 → 对象），实现在 Reservation.cpp（static 工厂，同 Computer::fromCsv）
    static Reservation fromCsv(const std::string& line);
};



#endif //RESERVATION_H  // 头文件保护宏结束
