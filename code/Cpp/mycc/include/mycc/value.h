#pragma once

#include <cstddef>
#include <ostream>
#include <string>
#include <variant>

namespace Mycc {

// 运行期值类型——五种之一
struct Nil {};   // 用空类区分 nil 和 false
inline bool operator==(Nil, Nil) { return true; }

using Value = std::variant<
    Nil,            // index 0
    bool,           // index 1
    double,         // index 2
    std::string,    // index 3
    std::size_t     // index 4：函数索引（指向 functions 表）
>;

// ----- 真值判定（控制流用）-----
inline bool isTruthy(const Value& v) {
    if (std::holds_alternative<Nil>(v))    return false;
    if (std::holds_alternative<bool>(v))   return std::get<bool>(v);
    if (std::holds_alternative<double>(v)) return std::get<double>(v) != 0.0;
    return true;   // string 非空、function 总是真
}

// ----- 调试输出 -----
inline std::ostream& operator<<(std::ostream& os, const Value& v) {
    std::visit([&](auto&& x) {
        using T = std::decay_t<decltype(x)>;
        if constexpr (std::is_same_v<T, Nil>)         os << "nil";
        else if constexpr (std::is_same_v<T, bool>)   os << (x ? "true" : "false");
        else if constexpr (std::is_same_v<T, double>) os << x;
        else if constexpr (std::is_same_v<T, std::string>) os << x;
        else if constexpr (std::is_same_v<T, std::size_t>) os << "<fn#" << x << ">";
    }, v);
    return os;
}

}  // namespace Mycc
