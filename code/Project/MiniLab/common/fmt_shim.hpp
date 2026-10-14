// common/fmt_shim.hpp — 自制 fmt 兼容层
// 零外部依赖，提供 fmt::print / fmt::format / fmt::fg / fmt::color / fmt::join 子集
// 教学目的够用，不是生产级实现。详见仓库 README。
#pragma once
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

namespace fmt {

// -------- color --------
namespace color {
    enum Color : int {
        cyan      = 36,
        yellow    = 33,
        red       = 31,
        green     = 32,
        dim_gray  = 90,
    };
}
struct text_style { int code = 0; };
inline text_style fg(color::Color c) { return {static_cast<int>(c)}; }

// -------- to_s：类型转字符串 --------
namespace _impl {

template <class T>
std::string to_s(T&& v, const std::string& spec = "");

template <class T>
std::string to_s_default(const T& v) {
    using D = std::decay_t<T>;
    if constexpr (std::is_same_v<D, bool>) return v ? "true" : "false";
    else if constexpr (std::is_same_v<D, char>)                    { return std::string(1, v); }
    else if constexpr (std::is_same_v<D, const char*>)             { return v ? std::string(v) : std::string(); }
    else if constexpr (std::is_same_v<D, char*>)                   { return v ? std::string(v) : std::string(); }
    else if constexpr (std::is_same_v<D, std::string>)             { return v; }
    else if constexpr (std::is_same_v<D, std::string_view>)        { return std::string(v); }
    else if constexpr (std::is_pointer_v<D>) {
        char buf[32]; std::snprintf(buf, sizeof(buf), "%p", (const void*)v); return buf;
    }
    else if constexpr (std::is_integral_v<D>) {
        return std::to_string(v);
    }
    else if constexpr (std::is_floating_point_v<D>) {
        char buf[64]; std::snprintf(buf, sizeof(buf), "%g", (double)v); return buf;
    }
    else {
        std::ostringstream ss; ss << v; return ss.str();
    }
}

// 解析简单格式说明符 "spec"（已去掉大括号与 `:`）
// 支持：
//   .Nf          → 浮点精度
//   .Ng          → 浮点有效位
//   x / X        → 16 进制
//   b / o        → 二 / 八 进制
//   d            → 十进制
//   <N / >N / ^N → 左/右/居中对齐，宽度 N
//   N 直接数字   → 宽度 N
//   其它一律落到 to_s_default
template <class T>
std::string to_s_fmt(const T& v, std::string_view spec) {
    using D = std::decay_t<T>;

    // 对齐与宽度（最简单处理：解析前缀 '<'/'>'/'^' + 数字）
    int width = 0;
    char align = 0;
    size_t i = 0;
    if (i < spec.size() && (spec[i] == '<' || spec[i] == '>' || spec[i] == '^')) {
        align = spec[i++];
    }
    while (i < spec.size() && spec[i] >= '0' && spec[i] <= '9') {
        width = width * 10 + (spec[i++] - '0');
    }

    std::string body;

    // 剩下的是类型描述符或浮点精度
    if (i < spec.size()) {
        char tail = spec[spec.size() - 1];
        // 浮点 .Nf / .Ng
        if (spec[i] == '.') {
            // 解析 .N
            ++i;
            int prec = 0;
            while (i < spec.size() && spec[i] >= '0' && spec[i] <= '9')
                prec = prec * 10 + (spec[i++] - '0');
            char kind = (i < spec.size()) ? spec[i] : 'f';
            if constexpr (std::is_floating_point_v<D> || std::is_integral_v<D>) {
                char fmt_buf[16];
                std::snprintf(fmt_buf, sizeof(fmt_buf), "%%.%d%c", prec, kind);
                char val_buf[64];
                std::snprintf(val_buf, sizeof(val_buf), fmt_buf, (double)v);
                body = val_buf;
            } else {
                body = to_s_default(v);
            }
        }
        // 十进制 / 16 进制 / 8 进制 / 二进制
        else if (tail == 'd' || tail == 'x' || tail == 'X' || tail == 'o' || tail == 'b') {
            if constexpr (std::is_integral_v<D>) {
                char fmt_buf[8], val_buf[64];
                if (tail == 'b') {
                    // 二进制特殊处理
                    unsigned long long u = (unsigned long long)v;
                    body = u ? "" : "0";
                    while (u) { body = char('0' + (u & 1)) + body; u >>= 1; }
                } else {
                    const char* spec_c = tail == 'd' ? "%lld" :
                                         tail == 'x' ? "%llx" :
                                         tail == 'X' ? "%llX" : "%llo";
                    std::snprintf(val_buf, sizeof(val_buf), spec_c, (long long)v);
                    body = val_buf;
                }
            } else body = to_s_default(v);
        }
        else {
            body = to_s_default(v);
        }
    } else {
        body = to_s_default(v);
    }

    // 应用对齐
    if (width > 0 && (int)body.size() < width) {
        int pad = width - (int)body.size();
        char fill = ' ';
        if (align == '<')
            body.append(pad, fill);
        else if (align == '^') {
            int l = pad / 2, r = pad - l;
            body = std::string(l, fill) + body + std::string(r, fill);
        } else  // '>' 或 默认（右对齐）
            body.insert(0, pad, fill);
    }

    return body;
}

template <class T>
std::string to_s(T&& v, const std::string& spec) {
    if (spec.empty()) return to_s_default(std::forward<T>(v));
    return to_s_fmt(v, std::string_view(spec));
}

// Vformat：按 cells 替换 {} 或 {:spec}
inline std::string vformat(std::string_view f, const std::vector<std::string>& cells) {
    std::string out;
    out.reserve(f.size() + cells.size() * 8);
    size_t idx = 0;
    for (size_t i = 0; i < f.size(); ) {
        char c = f[i];
        if (c == '{' && i + 1 < f.size() && f[i + 1] == '{') { out += '{'; i += 2; continue; }
        if (c == '}' && i + 1 < f.size() && f[i + 1] == '}') { out += '}'; i += 2; continue; }
        if (c == '{') {
            // find matching }
            size_t end = f.find('}', i);
            if (end == std::string_view::npos) { out += c; ++i; continue; }
            // ignore spec part (已经在 to_s 调用时处理)
            if (idx < cells.size()) out += cells[idx++];
            i = end + 1;
        } else {
            out += c; ++i;
        }
    }
    return out;
}

// 把参数包展开成 (格式说明符解析后的) 字符串数组
template <class... Args, size_t... I>
std::vector<std::string> make_cells(std::string_view f, std::index_sequence<I...>,
                                    Args&&... args) {
    // 扫描格式字符串，提取每个 {} 内部的 spec 字符串
    std::vector<std::string> specs;
    for (size_t i = 0; i < f.size(); ) {
        if (f[i] == '{' && i + 1 < f.size() && f[i + 1] == '{') { i += 2; continue; }
        if (f[i] == '{') {
            size_t end = f.find('}', i);
            if (end == std::string_view::npos) break;
            std::string s;
            size_t colon = f.find(':', i);
            if (colon != std::string_view::npos && colon < end)
                s.assign(f.data() + colon + 1, end - colon - 1);
            specs.push_back(std::move(s));
            i = end + 1;
        } else ++i;
    }
    auto arg_tuple = std::forward_as_tuple(std::forward<Args>(args)...);
    std::vector<std::string> cells;
    cells.reserve(sizeof...(Args));
    (cells.push_back(to_s(std::get<I>(arg_tuple),
                          I < specs.size() ? specs[I] : std::string())), ...);
    return cells;
}

}  // namespace _impl

// -------- 核心 API --------
template <class... Args>
inline std::string format(std::string_view f, Args&&... args) {
    auto cells = _impl::make_cells(f, std::index_sequence_for<Args...>{},
                                   std::forward<Args>(args)...);
    return _impl::vformat(f, cells);
}

inline std::string format(std::string_view f) { return std::string(f); }

template <class... Args>
inline void print(std::string_view f, Args&&... args) {
    std::fputs(format(f, std::forward<Args>(args)...).c_str(), stdout);
}
template <class... Args>
inline void print(std::FILE* fp, std::string_view f, Args&&... args) {
    std::fputs(format(f, std::forward<Args>(args)...).c_str(), fp);
}

// 带颜色打印：输出 ANSI 转义码（终端非 TTY 时也打印转义码，教学够用）
template <class... Args>
inline void print(text_style ts, std::string_view f, Args&&... args) {
    char open[8]; std::snprintf(open, sizeof(open), "\x1b[%dm", ts.code);
    std::fputs(open, stdout);
    std::fputs(format(f, std::forward<Args>(args)...).c_str(), stdout);
    std::fputs("\x1b[0m", stdout);
}

// -------- fmt::join --------
template <class Range>
inline std::string join(const Range& r, std::string_view sep) {
    std::string s; bool first = true;
    for (const auto& x : r) {
        if (!first) s += sep;
        s += _impl::to_s_default(x);
        first = false;
    }
    return s;
}

}  // namespace fmt
