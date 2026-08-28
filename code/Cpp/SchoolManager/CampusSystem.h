//
// Created by 杨充 on 2026/5/28.
//

#ifndef CAMPUSSYSTEM_H
#define CAMPUSSYSTEM_H
#include "User.h"
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "Computer.h"
#include "Reservation.h"


class Speech;

class CampusSystem {
private:
    std::map<std::string, std::shared_ptr<User>> users;
    std::map<int, Computer> rooms;
    std::vector<Reservation> reservations;
    std::set<int> reservedRooms;
    int nextResId = 1;
    // 存储键值对（key-value pairs）。与 std::map 不同，std::multimap 允许键（key）重复，即多个值可以关联到同一个键。
    std::multimap<int, Speech> speeches;
public:
    CampusSystem();
    bool addUser(std::shared_ptr<User> u);
    std::shared_ptr<User> login(const std::string& id, const std::string& pwd);

    // student
    void listRooms() const;
    bool reserveRoom(const std::string& sid, int roomId, const std::string& date);
    bool cancelReservation(const std::string& sid, int resId);
    bool signupSpeech(const std::string& sid, const std::string& topic, int round);

    // teacher
    void listPendingReservations() const;                       // ⭐ Teacher 业务方法 1
    bool reviewReservation(int resId, bool approved);           // ⭐ Teacher 业务方法 2
    void scoreSpeech(const std::string& sid, int round, double score);   // ⭐ 业务方法 3
    void rankSpeechesByScore(int round) const;     // ⭐ 业务方法 4：lambda 排序高光

    // admin
    bool addRoom(const Computer& c);     // ⭐ Admin 业务方法
    void statistics() const;     // ⭐ 业务方法：lambda 三连 accumulate
};



#endif //CAMPUSSYSTEM_H
