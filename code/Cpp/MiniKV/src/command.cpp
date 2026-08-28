


#include <string>
#include "mkv/command.hpp"  // 假设 CmdType 定义在 command.hpp 中

namespace mkv {

    CmdType parseCmdType(const std::string& s) {
        // 支持大小写不敏感
        std::string up;
        up.reserve(s.size());
        for (char c : s) up.push_back(static_cast<char>(std::toupper(c)));

        if (up == "SET")    return CmdType::Set;
        if (up == "GET")    return CmdType::Get;
        if (up == "DEL")    return CmdType::Del;
        if (up == "EXPIRE") return CmdType::Expire;
        if (up == "TTL")    return CmdType::Ttl;
        if (up == "KEYS")   return CmdType::Keys;
        if (up == "SAVE")   return CmdType::Save;
        if (up == "EXIT")   return CmdType::Exit;
        return CmdType::Unknown;
    }

    // 把一行命令切成多个 string_view，全部指向同一个底层 buffer
    std::vector<std::string_view> tokenize(std::string_view line) {
        std::vector<std::string_view> out;
        size_t i = 0;
        while (i < line.size()) {
            // 跳过前导空白（空格、tab、\r、\n 都算）
            while (i < line.size() && std::isspace(static_cast<unsigned char>(line[i]))) ++i;
            size_t start = i;
            // 吞掉一个 token 直到下一个空白
            while (i < line.size() && !std::isspace(static_cast<unsigned char>(line[i]))) ++i;
            // substr(pos, len) 返回的是新的 string_view，不拷贝底层字节
            if (start < i) out.emplace_back(line.substr(start, i - start));
        }
        return out;
    }
}



