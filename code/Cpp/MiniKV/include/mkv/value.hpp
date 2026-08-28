#pragma once

#include <variant>
#include <string>
#include <cstdint>
#include <string_view>
#include <cctype>
#include <stdexcept>

namespace mkv {

    // 占位类型，表示"没有值"
    struct Null{};

    inline bool operator==(Null, Null) noexcept { return true; }
    using Value = std::variant<Null, bool, std::int64_t, double, std::string>;

    // 类型查询（封装一下让调用方好看）
    inline bool isNull  (const Value& v) noexcept { return std::holds_alternative<Null>(v); }
    inline bool isBool  (const Value& v) noexcept { return std::holds_alternative<bool>(v); }
    inline bool isInt   (const Value& v) noexcept { return std::holds_alternative<std::int64_t>(v); }
    inline bool isDouble(const Value& v) noexcept { return std::holds_alternative<double>(v); }
    inline bool isString(const Value& v) noexcept { return std::holds_alternative<std::string>(v); }

    // 只写"函数长啥样"，不写函数体 —— 具体实现挪到 value.cpp
    std::string formatValue(const Value& v);

    // 要能自动判断 `"42"` 是 int、`"3.14"` 是 double、`"hello"` 是 string。这是"动态类型"在静态语言里的真实落地姿势。
    Value valueFromToken(std::string_view s);

}