#include "mkv/command.hpp"

#include <cctype>      // std::toupper / std::isspace / std::isdigit
#include <string>      // std::string / to_string
#include <memory>      // std::make_unique

#include "mkv/store.hpp"    // 需要 Store 的完整定义才能调它的方法
// 头文件里 Store 只是前置声明（引用参数够用）；这里要调 store.set 等方法 ——
// 必须完整定义 —— "声明用前向、实现处 include"的分层包含策略
#include "mkv/errors.hpp"   // CmdSyntaxError

namespace mkv {

// 把命令字符串转成强类型枚举，支持大小写不敏感
// 【为什么先规范化】"set/SET/Set" 是同一个命令（用户习惯差异不该报错）——
//   统一转大写后一次比较命中，避免每个命令写 3 种拼写分支
CmdType parseCmdType(const std::string& s) {
    std::string up;
    up.reserve(s.size());      // 预分配：逐字符追加不扩容
    for (char c : s) up.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(c))));
    // 【两重转型】unsigned char 转（isdigit/toupper 参数为负是 UB）→
    //   toupper 返回 int 再收回 char（可能超出 char 范围的输入已排除）

    if (up == "SET")    return CmdType::Set;
    if (up == "GET")    return CmdType::Get;
    if (up == "DEL")    return CmdType::Del;
    if (up == "EXPIRE") return CmdType::Expire;
    if (up == "TTL")    return CmdType::Ttl;
    if (up == "KEYS")   return CmdType::Keys;
    if (up == "SAVE")   return CmdType::Save;
    if (up == "EXIT")   return CmdType::Exit;
    return CmdType::Unknown;
    // Unknown 哨兵：让"不认识的命令"也成为一种合法枚举值 ——
    // 调用方 switch 有明确的 default 语义，不靠异常表达流控
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
        // 【零拷贝切词的机制】string_view::substr 只做"指针+长度"算术 ——
        //   万次切词零堆分配；所有 view 都指向调用方的同一个 line buffer
        if (start < i) out.emplace_back(line.substr(start, i - start));
        // start < i 保证不产出空 token（连续空白被上面的跳过逻辑消化）
    }
    return out;
}

// ─── makeCommand 工厂：把 tokens 组装成具体 Command ───
std::unique_ptr<Command> makeCommand(const std::vector<std::string_view>& tokens) {
    // 空命令直接抛异常，避免下游 tokens[0] 越界
    if (tokens.empty()) throw CmdSyntaxError("empty command");

    // Lambda 局部辅助：断言参数个数。捕获 tokens 引用避免拷贝 vector
    // 【局部工具函数的抽象】7 个命令都要"校验参数个数 + 报命令名" ——
    //   抽成 lambda 一处复用（比每个 case 手写 if 干净 7 倍）
    auto needArgs = [&](size_t n, std::string_view name) {
        if (tokens.size() != n + 1) {   // +1 是命令名本身
            throw CmdSyntaxError(std::string(name) + " expects "
                                 + std::to_string(n) + " arg(s)");
        }
    };

    switch (parseCmdType(std::string(tokens[0]))) {
    // 【switch on 枚举】分发中枢 —— 工厂里唯一"认识所有命令"的地方；
    // 主流程从此与具体命令解耦（教程§05 的核心收益）
        case CmdType::Set: {
            needArgs(2, "SET");
            return std::make_unique<SetCmd>(
                std::string(tokens[1]),          // key（拷贝一份成 string 存起来）
                valueFromToken(tokens[2]),       // 推断类型后的 Value
                std::string(tokens[2]));         // 原始字符串（用于 toAofLine 序列化）
            // 【同一 token 两次拷贝】tokens[2] 既做"类型推断源"又做"AOF 原文" ——
            //   valueFromToken 拿 view 零拷贝推断，推断完才知道要不要存 string
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
                // 【异常翻译】标准库异常（面向机器）→ 项目异常（面向用户）——
                //   顶层 REPL 只 catch KvError，统一口径；"bad lexical cast" 这种
                //   术语不该出现在用户面前
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
            // Unknown / Exit 落到这里：不认识的命令报语法错
            // （EXIT 在 main 里提前拦截，根本进不了工厂）
            throw CmdSyntaxError("unknown command: " + std::string(tokens[0]));
    }
}

// ─── 各命令 execute 实现 ──────────────────────────
// 【共同套路】调 Store 方法 → 把结果翻译成 Redis 风格回复文本。
//   命令类只做"翻译"，一切并发/过期语义都在 Store —— 职责分层。

std::string SetCmd::execute(Store& store) {
    store.set(key_, value_);
    return "OK";
    // 注意：value_ 被 move 进 store（set 按值收 + 内部 move）——
    // 同一命令对象不能 execute 两次（值已空壳），当前用法一次即弃，安全
}

std::string GetCmd::execute(Store& store) {
    auto entry = store.get(key_);       // EntryPtr = shared_ptr<Entry>
    if (!entry) return "(nil)";         // 不存在/已过期统一 nil（Redis 协议）
    return formatValue(entry->value);   // 按实际类型格式化输出
}

std::string DelCmd::execute(Store& store) {
    auto n = store.del(key_);
    return "(integer) " + std::to_string(n);   // 1=删了 0=本来就没有
}

std::string ExpireCmd::execute(Store& store) {
    return store.expire(key_, seconds_) ? "OK" : "(integer) 0";
    // key 不存在返回 0（Redis 协议：EXPIRE 对不存在的 key 返回 0）
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
        if (i) out += "\n";             // 首行不换行（前一行结尾才补 \n）
        out += std::to_string(i + 1) + ") \"" + keys[i] + "\"";
        // Redis 风格编号列表：1) "age"  2) "name" —— 教学版省略 pattern
        // 通配符匹配（pattern_ 目前仅占位保留）
    }
    return out;
}

std::string SaveCmd::execute(Store& store) {
    (void)store;   // SAVE 不改数据，刷盘动作由 main 循环里对 SAVE 单独调用 aof.flush() 完成
    // 【(void) 抑制未用参数警告】接口要求 execute(Store&) 统一签名 ——
    //   SAVE 用不上 store，转型 void 告诉编译器"有意不用"
    // 【设计辨析】刷盘是 IO 动作，命令对象只懂"对 store 做什么"——
    //   把 aof.flush 放 main 是"命令层不掺持久化"的职责切分
    return "OK";
}

}  // namespace mkv
