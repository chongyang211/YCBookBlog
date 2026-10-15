#pragma once

#include <cstddef>   // std::size_t（函数索引型）
#include <ostream>
#include <string>
#include <variant>

namespace Mycc {

// ============================================================================
// 【模块定位】value.h —— 运行期值（VM 栈上的"通货"，variant 五型）
// ----------------------------------------------------------------------------
// 【设计思想】编译期类型（Type 枚举）只用于检查；运行期值用另一个 variant：
//   多了 Nil（语句的"无值"）和 size_t（函数索引 —— 函数是一等公民，
//   能被 LOAD_GLOBAL 推上栈、被 CALL 调用）。
// 【与 Token::value / Constant 的对照】三个 variant 各管一段：
//   Token 载荷（monostate/double/string）管词法、Constant 管 bytecode 常量池、
//   Value 管运行期 —— 各阶段"值"的形态不同是编译器的常态。
// ============================================================================

// 运行期值类型——五种之一
struct Nil {};   // 用空类区分 nil 和 false
// 【为什么要区分】若用 bool 同时表达 nil/false，print 的输出没法区分
//   "函数没返回值"和"返回了 false" —— 语义上二者完全不同
inline bool operator==(Nil, Nil) { return true; }
// 空 struct 的 == 手写（同 MiniKV/JsonNode 的 Null 同款处理）：
//   variant 的相等比较需要每个备选型可比较

using Value = std::variant<
    Nil,            // index 0
    bool,           // index 1
    double,         // index 2
    std::string,    // index 3
    std::size_t     // index 4：函数索引（指向 functions 表）
>;
// 【函数索引的本质】函数不是数据、是"可调用的地址" —— 用 fnTable_ 的下标
//   当"函数值"（对应真实 VM 的函数指针/闭包结构）。LOAD_GLOBAL 遇到
//   函数名就 push 这个索引，CALL 拿索引查表 —— "函数是一等公民"的最小实现

// ----- 真值判定（控制流用）-----
inline bool isTruthy(const Value& v) {
    if (std::holds_alternative<Nil>(v))    return false;
    if (std::holds_alternative<bool>(v))   return std::get<bool>(v);
    if (std::holds_alternative<double>(v)) return std::get<double>(v) != 0.0;
    return true;   // string 非空、function 总是真
    // 【运行期的宽容 vs 检查期的严格】TypeChecker 已保证 if/while 条件是
    //   Bool、&& || 两侧是 Bool —— isTruthy 理论上只接 Bool。
    //   但 NOT 指令（!x）也走这里，留全类型分支是防御性（错误消息更友好
    //   而不是 crash）—— "严格类型检查 + 运行期兜底"的双保险分层
}

// ----- 调试输出 -----
inline std::ostream& operator<<(std::ostream& os, const Value& v) {
    // print 指令的最终输出格式化：visit + if constexpr 编译期分派
    //   （同 JsonManager formatValue 的写法 —— variant 三件套惯例）
    std::visit([&](auto&& x) {
        using T = std::decay_t<decltype(x)>;
        if constexpr (std::is_same_v<T, Nil>)         os << "nil";
        else if constexpr (std::is_same_v<T, bool>)   os << (x ? "true" : "false");
        else if constexpr (std::is_same_v<T, double>) os << x;
        else if constexpr (std::is_same_v<T, std::string>) os << x;
        else if constexpr (std::is_same_v<T, std::size_t>) os << "<fn#" << x << ">";
        // 函数值打印为 <fn#2>：函数本体不可"显示"，只展示索引（调试可读）
    }, v);
    return os;
}

}  // namespace Mycc
