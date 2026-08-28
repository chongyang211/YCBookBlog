//
// Created by 杨充 on 2026/5/28.
//

#include "CampusSystem.h"

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <numeric>

#include "Speech.h"

CampusSystem::CampusSystem() {
    std::cout << "[System] 校园系统启动\n";
}


bool CampusSystem::addUser(std::shared_ptr<User> u) {
    std::string id = u->getId();
    if (users.count(id) > 0) {
        std::cout << "[System] 用户 " << u->getId() << " 已存在\n";
        return false;
    }
    users[id] = u;
    std::cout << "[System] 添加用户 " << u->getId() << " 成功\n";
    return true;
}

std::shared_ptr<User> CampusSystem::login(const std::string &id, const std::string &pwd) {
    auto it = users.find(id);
    if (it == users.end()) {
        std::cout << "[System] 账号不存在\n";
        return nullptr;
    }
    if (!it->second->verify(pwd)) {
        std::cout << "[System] 密码错误\n";
        return nullptr;
    }
    return it->second;
}

// 列出所有机房的信息。const 表示该函数不会修改类的成员变量。
void CampusSystem::listRooms() const {
    std::cout << "\n=== 机房列表 ===\n";
    // 输出机房列表的表头。std::left：设置输出左对齐。std::setw(n)：设置字段宽度为 n 个字符。
    // 表头包括四列：编号、容量、配置 和 状态。
    std::cout << std::left << std::setw(6) << "编号" << std::setw(8) << "容量"
         << std::setw(20) << "配置" << std::setw(8) << "状态\n";
    std::cout << std::string(42, '-') << "\n";
    if (rooms.empty()) { std::cout << "（暂无机房）\n"; return; }
    for (const auto& [id, room] : rooms) {
        bool occupied = reservedRooms.count(id) > 0;
        // bool occupied = false;
        std::cout << std::left << std::setw(6) << id << std::setw(8) << room.capacity
            << std::setw(20) << room.spec
            << (occupied ? "占用中" : "空闲") << "\n";
    }
}

bool CampusSystem::reserveRoom(const std::string &sid, int roomId, const std::string &date) {
    if (rooms.find(roomId) == rooms.end()) {
        std::cout << "[预约] 机房不存在\n";
        return false;
    }
    if (reservedRooms.count(roomId) > 0) {        // ⭐ 新增：校验未被占用
        std::cout << "[预约] 该机房已被预约\n";
        return false;
    }
    Reservation r(nextResId++, sid, roomId, date);
    reservations.push_back(r);    // 只塞 vector
    reservedRooms.insert(roomId);
    std::cout << "[预约] 提交成功，预约号 " << r.resId << "\n";
    return true;
}

bool CampusSystem::cancelReservation(const std::string &sid, int resId) {
    auto it = std::find_if(reservations.begin(),reservations.end(), [resId,&sid](const Reservation& r) {
        return r.resId == resId && r.studentId == sid;
    });
    if (it == reservations.end()) {
        std::cout << "[取消] 预约不存在或不属于你\n";
        return false;
    }
    if (it->status != ResStatus::Pending) {
        std::cout << "[取消] 该预约已被审核，无法取消\n";
        return false;
    }
    it->status = ResStatus::Cancelled;
    reservedRooms.erase(it->computerId);    // ⭐ 同步清理 set（取消后机房空闲）
    std::cout << "[取消] 已取消\n";
    return true;
}


bool CampusSystem::signupSpeech(const std::string &sid, const std::string &topic, int round) {
    if (round != 1 && round != 2) {
        std::cout << "[演讲] round 只能是 1 或 2\n";
        return false;
    }
    // ⭐ multimap 允许同 key 多 value：一个 round 可以有很多人报名
    speeches.insert({round,Speech(sid, topic, round)});
    std::cout << "[演讲] 报名成功 - 第 " << round << " 轮 - " << topic << "\n";
    return true;
}

void CampusSystem::listPendingReservations() const {
    std::cout << "\n=== 待审核预约 ===\n";
    // ⭐ count_if 先看有几个（没有就提前返回）
    auto pendingCount = std::count_if(reservations.begin(), reservations.end(),
        [](const Reservation& r) { return r.status == ResStatus::Pending; });

    if (pendingCount == 0) { std::cout << "（无待审）\n"; return; }

    for (const auto& r : reservations) {
        if (r.status != ResStatus::Pending) continue;
        std::cout << "  预约 " << r.resId << " | 学生 " << r.studentId
             << " | 机房 " << r.computerId << " | 日期 " << r.date << "\n";
    }
}

bool CampusSystem::reviewReservation(int resId, bool approved) {
    auto it = std::find_if(reservations.begin(), reservations.end(),
        [resId](const Reservation& r) { return r.resId == resId; });
    if (it == reservations.end()) {
        std::cout << "[审核] 预约不存在\n";
        return false;
    }
    if (it->status != ResStatus::Pending) {
        std::cout << "[审核] 该预约已被处理过\n";
        return false;
    }

    it->status = approved ? ResStatus::Approved : ResStatus::Rejected;
    if (!approved) reservedRooms.erase(it->computerId);   // ⭐ 拒绝时释放占用
    std::cout << "[审核] 预约 " << resId << " - " << it->statusText() << "\n";
    return true;
}

void CampusSystem::scoreSpeech(const std::string &sid, int round, double score) {
    // ⭐ multimap::equal_range：拿到 round 这个 key 的所有 entry 的 [first, last)
    auto range = speeches.equal_range(round);
    for (auto it = range.first; it != range.second; ++it) {
        if (it->second.studentId == sid) {
            it->second.score = score;
            std::cout << "[评分] " << sid << " 第 " << round
                 << " 轮 - " << score << " 分\n";
            return;
        }
    }
    std::cout << "[评分] 未找到该报名\n";
}

void CampusSystem::rankSpeechesByScore(int round) const {
    // 1. 把 multimap 中 round 对应的 entry 抽到 vector（multimap 不能直接 sort）
    std::vector<Speech> arr;
    auto range = speeches.equal_range(round);
    for (auto it = range.first; it != range.second; ++it) {
        arr.push_back(it->second);
    }

    if (arr.empty()) { std::cout << "[排名] 第 " << round << " 轮无人报名\n"; return; }

    // 2. ⭐ lambda 比较器：分数高的在前
    std::sort(arr.begin(), arr.end(),
        [](const Speech& a, const Speech& b) { return a.score > b.score; });

    // 3. 输出排名
    std::cout << "\n=== 第 " << round << " 轮排名 ===\n";
    int rank = 1;
    for (const auto& s : arr) {
        std::cout << "  " << rank++ << ". " << s.studentId
             << " - " << s.topic << " - " << s.score << " 分\n";
    }
}

bool CampusSystem::addRoom(const Computer &c) {
    if (rooms.count(c.id) > 0) {
        std::cout << "[Admin] 机房 " << c.id << " 已存在\n";
        return false;
    }
    rooms[c.id] = c;
    std::cout << "[Admin] 添加机房 " << c.id << " 成功\n";
    return true;
}

void CampusSystem::statistics() const {
    std::cout << "\n=== 数据统计 ===\n";
    std::cout << "用户总数: " << users.size() << "\n";
    std::cout << "机房总数: " << rooms.size() << "\n";

    // ⭐ accumulate + lambda 实现"按身份分类计数"
    int students = std::accumulate(users.begin(), users.end(), 0,
        [](int sum, const auto& kv) {
            return sum + (kv.second->roleTag() == 'S' ? 1 : 0);
        });
    int teachers = std::accumulate(users.begin(), users.end(), 0,
        [](int sum, const auto& kv) {
            return sum + (kv.second->roleTag() == 'T' ? 1 : 0);
        });

    std::cout << "  学生: " << students << " | 教师: " << teachers
         << " | 管理员: " << users.size() - students - teachers << "\n";

    std::cout << "预约总数: " << reservations.size() << "\n";
    int pending = std::count_if(reservations.begin(), reservations.end(),
        [](const Reservation& r) { return r.status == ResStatus::Pending; });
    std::cout << "  待审: " << pending
         << " | 已审: " << reservations.size() - pending << "\n";

    std::cout << "演讲报名: " << speeches.size() << " 人次\n";
    std::cout << "  第 1 轮: " << speeches.count(1)
         << " | 第 2 轮: " << speeches.count(2) << "\n";
}

