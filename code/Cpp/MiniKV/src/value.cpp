// 实现文件必须先包含自己的头文件
#include "mkv/value.hpp"

// namespace mkv 和头文件保持一致，否则链接找不到
namespace mkv {

    // formatValue 的真正实现（对应 value.hpp 里的那行声明）
    std::string formatValue(const Value &v) {
        return std::visit([](const auto &x) -> std::string {
            using T = std::decay_t<decltype(x)>;
            // 覆盖5个分支
            if constexpr (std::is_same_v<T, Null>) return "(nil)";
            else if constexpr (std::is_same_v<T, bool>) return x ? "true" : "false";
            else if constexpr (std::is_same_v<T, std::int64_t>) return "(integer) " + std::to_string(x);
            else if constexpr (std::is_same_v<T, double>) return "(double) " + std::to_string(x);
            else if constexpr (std::is_same_v<T, std::string>) return "\"" + x + "\"";
        }, v);
    }

    // valueFromToken 的真正实现（对应 value.hpp 里的那行声明）
    // 规则：
    //   "true"/"false" -> bool
    //   "nil"          -> Null
    //   全数字（可带负号）-> int64
    //   含小数点且能 parse -> double
    //   其他 -> string
    Value valueFromToken(std::string_view s) {
        if (s == "true") return true;
        if (s == "false") return false;
        if (s == "nil") return Null{};
        // ─── 分支 2：试探 int64 ───────────────────
        if (!s.empty()) {
            bool allDigit = true;
            // 允许开头一个符号
            size_t start = (s[0] == '-' ? 1 : 0);
            for (size_t i = start; i < s.size(); ++i) {
                if (!std::isdigit(static_cast<unsigned char>(s[i]))) {
                    allDigit = false;
                    break;
                }
            }
            if (allDigit && start < s.size()) {
                // stoll 只能吃 std::string，不能直接吃 string_view，此处一次拷贝可接受
                try { return static_cast<std::int64_t>(std::stoll(std::string(s))); }
                catch (...) { /* 溢出等异常吞掉，落到下一分支 */ }
            }
        }

        // ─── 分支 3：试探 double ──────────────────
        if (s.find('.') != std::string_view::npos) {
            try { return std::stod(std::string(s)); }
            catch (...) { /* 解析失败，落到兜底 */ }
        }

        // ─── 分支 4：兜底当字符串 ─────────────────
        return std::string(s);
    }
}
