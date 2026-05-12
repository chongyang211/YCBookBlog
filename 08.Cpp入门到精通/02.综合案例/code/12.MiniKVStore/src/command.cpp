// command.cpp —— 命令解析、tokenize、各命令的 execute 实现
//
// 设计决策：
// - parseCmdType 对外暴露 noexcept，因为它只是字符串比较；
// - tokenize 返回 string_view 切片，零拷贝（注意调用方保证 line 生命周期）；
// - makeCommand 把所有 std 的 stoi/stod 异常翻译成 CmdSyntaxError，统一错误出口。

#include "mkv/command.hpp"
#include "mkv/store.hpp"
#include "mkv/errors.hpp"

#include <cctype>
#include <sstream>
#include <stdexcept>
#include <string>

namespace mkv {

CmdType parseCmdType(std::string_view s) noexcept {
    // 手写大写比较，避免在 noexcept 环境里抛异常（理论上 push_back 可能抛 bad_alloc，
    // 这里用栈缓冲规避；命令名都很短）。
    char buf[16] = {};
    if (s.size() >= sizeof(buf)) return CmdType::Unknown;
    for (size_t i = 0; i < s.size(); ++i) {
        buf[i] = static_cast<char>(std::toupper(static_cast<unsigned char>(s[i])));
    }
    std::string_view up(buf, s.size());

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

std::vector<std::string_view> tokenize(std::string_view line) {
    std::vector<std::string_view> out;
    size_t i = 0;
    while (i < line.size()) {
        while (i < line.size() && std::isspace(static_cast<unsigned char>(line[i]))) ++i;
        size_t start = i;
        while (i < line.size() && !std::isspace(static_cast<unsigned char>(line[i]))) ++i;
        if (start < i) out.emplace_back(line.substr(start, i - start));
    }
    return out;
}

// ============== execute 实现 ==============
std::string SetCmd::execute(Store& store) {
    store.set(key_, value_);
    return "OK";
}

std::string GetCmd::execute(Store& store) {
    auto entry = store.get(key_);
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
    auto t = store.ttl(key_);
    return "(integer) " + std::to_string(t);
}

std::string KeysCmd::execute(Store& store) {
    auto ks = store.keys();
    if (ks.empty()) return "(empty)";
    std::ostringstream oss;
    for (size_t i = 0; i < ks.size(); ++i) {
        if (i) oss << "\n";
        oss << (i + 1) << ") \"" << ks[i] << "\"";
    }
    return oss.str();
}

std::string SaveCmd::execute(Store& /*store*/) {
    // 真正的 fsync 在 main 里做（拿得到 AofWriter）；这里只回 OK。
    // 为了不破坏 isWrite()=false 的语义，我们让 main 在收到 SAVE 时额外 fsync。
    return "OK";
}

// ============== 工厂函数 ==============
std::unique_ptr<Command> makeCommand(const std::vector<std::string_view>& tokens) {
    if (tokens.empty()) throw CmdSyntaxError("empty command");

    auto needArgs = [&](size_t n, std::string_view name) {
        if (tokens.size() != n + 1) {
            throw CmdSyntaxError(std::string(name) + " expects " +
                                 std::to_string(n) + " arg(s)");
        }
    };

    switch (parseCmdType(tokens[0])) {
        case CmdType::Set: {
            needArgs(2, "SET");
            return std::make_unique<SetCmd>(
                std::string(tokens[1]),
                valueFromToken(tokens[2]),
                std::string(tokens[2]));
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
            } catch (const std::exception&) {
                throw CmdSyntaxError("EXPIRE seconds must be integer, got: " +
                                     std::string(tokens[2]));
            }
            if (sec < 0) throw CmdSyntaxError("EXPIRE seconds must be non-negative");
            return std::make_unique<ExpireCmd>(std::string(tokens[1]), sec);
        }
        case CmdType::Ttl: {
            needArgs(1, "TTL");
            return std::make_unique<TtlCmd>(std::string(tokens[1]));
        }
        case CmdType::Keys: {
            // KEYS 接受 0 个或 1 个通配符参数（本案例只支持 "*"，行为一致）
            if (tokens.size() > 2) {
                throw CmdSyntaxError("KEYS expects 0 or 1 arg");
            }
            return std::make_unique<KeysCmd>();
        }
        case CmdType::Save: {
            if (tokens.size() != 1) throw CmdSyntaxError("SAVE expects 0 arg");
            return std::make_unique<SaveCmd>();
        }
        case CmdType::Exit:
            throw CmdSyntaxError("EXIT should be handled in main loop");
        case CmdType::Unknown:
        default:
            throw CmdSyntaxError("unknown command: " + std::string(tokens[0]));
    }
}

}  // namespace mkv
