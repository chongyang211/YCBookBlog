// value.cpp —— Value 的格式化与字面量推断
//
// 实现说明：
// - formatValue 用 std::visit + 泛型 lambda + if constexpr，C++17 标准搭档；
// - valueFromToken 用朴素的 if-else 做类型推断，方便阅读，不追求 std::from_chars 那种极致。

#include "mkv/value.hpp"

#include <cctype>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace mkv {

std::string formatValue(const Value& v) {
    return std::visit([](const auto& x) -> std::string {
        using T = std::decay_t<decltype(x)>;
        if constexpr (std::is_same_v<T, Null>) {
            return "(nil)";
        } else if constexpr (std::is_same_v<T, bool>) {
            return x ? "true" : "false";
        } else if constexpr (std::is_same_v<T, std::int64_t>) {
            return "(integer) " + std::to_string(x);
        } else if constexpr (std::is_same_v<T, double>) {
            return "(double) " + std::to_string(x);
        } else if constexpr (std::is_same_v<T, std::string>) {
            return "\"" + x + "\"";
        }
    }, v);
}

Value valueFromToken(std::string_view s) {
    if (s == "true")  return true;
    if (s == "false") return false;
    if (s == "nil")   return Null{};

    // 试探 int64
    if (!s.empty()) {
        bool   allDigit = true;
        size_t start    = (s[0] == '-') ? 1 : 0;
        for (size_t i = start; i < s.size(); ++i) {
            if (!std::isdigit(static_cast<unsigned char>(s[i]))) {
                allDigit = false;
                break;
            }
        }
        if (allDigit && start < s.size()) {
            try {
                return static_cast<std::int64_t>(std::stoll(std::string(s)));
            } catch (...) { /* 落到下面 */ }
        }
    }

    // 试探 double
    if (s.find('.') != std::string_view::npos) {
        try {
            return std::stod(std::string(s));
        } catch (...) { /* 落到下面 */ }
    }

    // 兜底当字符串
    return std::string(s);
}

}  // namespace mkv
