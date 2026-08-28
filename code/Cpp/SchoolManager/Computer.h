//
// Created by 杨充 on 2026/5/28.
//

#ifndef COMPUTER_H
#define COMPUTER_H
#include <string>


class Computer {
public:
    int     id;            // 机房编号（主键）
    int     capacity;      // 容量（人数）
    std::string spec;      // 配置（如 "i7+RTX4060"）

    // 使用编译器生成的默认构造函数。
    Computer() = default;
    Computer(int id_, int cap, const std::string& s)
     : id(id_), capacity(cap), spec(s) {}

    std::string toCsv() const {
        return std::to_string(id) + "," + std::to_string(capacity) + "," + spec;
    }

    // 从 CSV 格式的字符串中解析并创建一个 Computer 对象。
    // 阶段 ⑦ 才会真正实现，先放声明
    static Computer fromCsv(const std::string& line);
};



#endif //COMPUTER_H
