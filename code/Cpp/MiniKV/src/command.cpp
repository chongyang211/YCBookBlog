#include "mkv/command.hpp"

#include <cctype>
#include <string>
#include <memory>

#include "mkv/store.hpp"    // 需要 Store 的完整定义才能调它的方法
#include "mkv/errors.hpp"   // CmdSyntaxError

namespace mkv {

// 把命令字符串转成强类型枚举，支持大小写不敏感
CmdType parseCmdType(const std::string& s) {
    std::string up;
    up.reserve(s.size());
    for (char c : s) up.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(c))));

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

// ─── makeCommand 工厂：把 tokens 组装成具体 Command ───
std::unique_ptr<Command> makeCommand(const std::vector<std::string_view>& tokens) {
    // 空命令直接抛异常，避免下游 tokens[0] 越界
    if (tokens.empty()) throw CmdSyntaxError("empty command");

    // Lambda 局部辅助：断言参数个数。捕获 tokens 引用避免拷贝 vector
    auto needArgs = [&](size_t n, std::string_view name) {
        if (tokens.size() != n + 1) {
            throw CmdSyntaxError(std::string(name) + " expects "
                                 + std::to_string(n) + " arg(s)");
        }
    };

    switch (parseCmdType(std::string(tokens[0]))) {
        case CmdType::Set: {
            needArgs(2, "SET");
            return std::make_unique<SetCmd>(
                std::string(tokens[1]),          // key（拷贝一份成 string 存起来）
                valueFromToken(tokens[2]),       // 推断类型后的 Value
                std::string(tokens[2]));         // 原始字符串（用于 toAofLine 序列化）
        }
        case CmdType::Get: {
            needArgs(1, "GET");
            return std::make_unique<GetCmd>(std::string(tokens[1]));
        }
        case CmdType::Del: {
            needArgs(1, "DEL");
            return std::make_unique<DelCmd>(std::string(tokens[1]));
        }
        case CmdType::Expire: {
            needArgs(2, "EXPIRE");
            int sec = 0;
            try {
                sec = std::stoi(std::string(tokens[2]));
            } catch (...) {
                // stoi 抛的是 std::invalid_argument，翻译成本项目异常体系
                throw CmdSyntaxError("EXPIRE seconds must be integer");
            }
            return std::make_unique<ExpireCmd>(std::string(tokens[1]), sec);
        }
        case CmdType::Ttl: {
            needArgs(1, "TTL");
            return std::make_unique<TtlCmd>(std::string(tokens[1]));
        }
        case CmdType::Keys: {
            needArgs(1, "KEYS");
            return std::make_unique<KeysCmd>(std::string(tokens[1]));
        }
        case CmdType::Save: {
            needArgs(0, "SAVE");
            return std::make_unique<SaveCmd>();
        }
        default:
            throw CmdSyntaxError("unknown command: " + std::string(tokens[0]));
    }
}

// ─── 各命令 execute 实现 ──────────────────────────

std::string SetCmd::execute(Store& store) {
    store.set(key_, value_);
    return "OK";
}

std::string GetCmd::execute(Store& store) {
    auto entry = store.get(key_);       // EntryPtr = shared_ptr<Entry>
    if (!entry) return "(nil)";
    return formatValue(entry->value);
}

std::string DelCmd::execute(Store& store) {
    auto n = store.del(key_);
    return "(integer) " + std::to_string(n);
}

std::string ExpireCmd::execute(Store& store) {
    return store.expire(key_, seconds_) ? "OK" : "(integer) 0";
}

std::string TtlCmd::execute(Store& store) {
    // -2 不存在、-1 永久、>=0 剩余秒，都按 Redis 风格直接输出
    return "(integer) " + std::to_string(store.ttl(key_));
}

std::string KeysCmd::execute(Store& store) {
    auto keys = store.keys();
    if (keys.empty()) return "(empty list or set)";

    std::string out;
    for (std::size_t i = 0; i < keys.size(); ++i) {
        if (i) out += "\n";
        out += std::to_string(i + 1) + ") \"" + keys[i] + "\"";
    }
    return out;
}

std::string SaveCmd::execute(Store& store) {
    (void)store;   // SAVE 不改数据，刷盘动作由 main 循环里对 SAVE 单独调用 aof.flush() 完成
    return "OK";
}

}  // namespace mkv
