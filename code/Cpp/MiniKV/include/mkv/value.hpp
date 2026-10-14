#pragma once

#include <variant>       // std::variant：类型安全的标签联合（C++17）
#include <string>        // std::string
#include <cstdint>       // std::int64_t：固定宽度整数
#include <string_view>   // std::string_view：零拷贝字符串视图
#include <cctype>        // std::isdigit
#include <stdexcept>     // std::stoll/stod 抛的异常家族

namespace mkv {

// ============================================================================
// 【模块定位】value.hpp —— Value 多类型值：variant 表达"一个 key 存多种类型"
// ----------------------------------------------------------------------------
// 【设计思想·教程§03 灵魂三问】
//   为什么 variant 而不是 union/继承？
//   ① union 无类型标签（取错类型是 UB）、不管理析构（string 进 union 要手写）；
//   ② 继承体系（IntValue/StringValue...）需要堆分配 + 虚表，小值太重；
//   ③ variant = "标签 + 存储 + 编译器生成的分派"，类型安全且值语义可拷贝。
// 【五路备选的顺序即索引】Null=0, bool=1, int64=2, double=3, string=4 ——
//   与 Redis 的五种回复类型对齐。
// ============================================================================

    // 占位类型，表示"没有值"
    struct Null{};
    // 【为什么需要空结构体】variant 必须用"类型"占位 —— std::nullptr_t 也行
    //   但语义混淆（那是"指针空"，不是"值空"）；自定义空类型让 Null 意图纯粹，
    //   且 sizeof(Null) == 1（variant 存标签绰绰有余）。

    inline bool operator==(Null, Null) noexcept { return true; }
    // 【手写 == 的原因】空结构体不会自动生成 ==（C++20 前）——
    //   显式定义"两个 Null 相等"，让 variant 的比较在 Null 分支可用。
    //   inline：头文件里定义，多个翻译单元包含不会重定义（ODR 安全）。

    using Value = std::variant<Null, bool, std::int64_t, double, std::string>;
    // 【五路备选详解】
    //   Null        —— GET 不存在时的值
    //   bool        —— "true"/"false" token
    //   int64_t     —— 固定 64 位（int 在不同平台宽度不一，int64_t 保证
    //                  AOF 重放后语义一致 —— 序列化格式跨平台稳定的根基）
    //   double      —— 含小数点的 token
    //   std::string —— 兜底分支
    //   【注意 bool 在 int64 前】variant 重载决议按"完全匹配"优先，
    //   token "true" 走显式字符串判断（valueFromToken），不会误入数字分支。

    // 类型查询（封装一下让调用方好看）
    // 【技术原理】holds_alternative<T>(v)：编译期已知 T 是否在备选集里，
    //   O(1) 读标签判断，不触碰值本身；noexcept 承诺不抛（查询无失败路径）
    inline bool isNull  (const Value& v) noexcept { return std::holds_alternative<Null>(v); }
    inline bool isBool  (const Value& v) noexcept { return std::holds_alternative<bool>(v); }
    inline bool isInt   (const Value& v) noexcept { return std::holds_alternative<std::int64_t>(v); }
    inline bool isDouble(const Value& v) noexcept { return std::holds_alternative<double>(v); }
    inline bool isString(const Value& v) noexcept { return std::holds_alternative<std::string>(v); }

    // 只写"函数长啥样"，不写函数体 —— 具体实现挪到 value.cpp
    std::string formatValue(const Value& v);
    // 序列化为 Redis 风格可读文本："(integer) 42"、"(double) 3.14"、
    //   "\"hello\""、"(nil)"、true/false —— 声明与实现分离（改实现不惊动依赖方）

    // 要能自动判断 `"42"` 是 int、`"3.14"` 是 double、`"hello"` 是 string。这是"动态类型"在静态语言里的真实落地姿势。
    Value valueFromToken(std::string_view s);
    // 【参数用 string_view 的理由】切词产物是 view（tokenize 零拷贝），
    //   直接传给推断函数避免逐 token 一次 string 拷贝 ——
    //   "调用链全程 view，只在确认要存时才拷贝成 string"的流水线设计。

}