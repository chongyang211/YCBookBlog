// mkv/value.hpp —— KV 存储的"值"类型
//
// 关键设计（详见 12.迷你KV存储引擎.md §03）：
// - 用 std::variant 而不是继承体系：在栈上保存"5 种类型之一"，零堆分配；
// - 用自定义 struct Null{} 而不是 std::monostate：业务可读性更好；
// - formatValue / valueFromToken 是"variant <-> 文本"的双向桥。

#pragma once

#include <variant>
#include <string>
#include <string_view>
#include <cstdint>

namespace mkv {

// 占位类型：表示"没有值"。比 std::monostate 语义更明确。
struct Null {};

inline bool operator==(Null, Null) noexcept { return true; }

using Value = std::variant<Null, bool, std::int64_t, double, std::string>;

// ============== 类型查询（noexcept 让调用方放心嵌入 noexcept 上下文） ==============
inline bool isNull  (const Value& v) noexcept { return std::holds_alternative<Null>(v); }
inline bool isBool  (const Value& v) noexcept { return std::holds_alternative<bool>(v); }
inline bool isInt   (const Value& v) noexcept { return std::holds_alternative<std::int64_t>(v); }
inline bool isDouble(const Value& v) noexcept { return std::holds_alternative<double>(v); }
inline bool isString(const Value& v) noexcept { return std::holds_alternative<std::string>(v); }

// 把 Value 转成可读字符串（用于 GET 命令返回）
std::string formatValue(const Value& v);

// 把命令行 token 推断为合适的 Value 类型
//   "true"/"false" -> bool
//   "nil"          -> Null
//   全数字          -> int64
//   含小数点能 parse -> double
//   其他           -> string
Value valueFromToken(std::string_view s);

}  // namespace mkv
