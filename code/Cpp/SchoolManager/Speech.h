//
// Created by 杨充 on 2026/5/28.
//

#pragma once
#include <string>


class Speech {
public:
    std::string studentId;     // 报名学生（外键关联 User）
    std::string topic;         // 主题
    int    round = 1;          // 轮次（1=初赛，2=复赛）
    double score = 0.0;        // 评分（默认 0，等教师评分）

    Speech() = default;
    Speech(const std::string& sid, const std::string& t, int r)
        : studentId(sid), topic(t), round(r) {}
};


