#pragma once

#include <iostream>     // std::cerr
#include <chrono>       // system_clock
#include <iomanip>      // std::put_time
#include <sstream>      // std::ostringstream
#include <ctime>        // localtime_r / std::tm

namespace mkv {

// ============================================================================
// 【模块定位】log.hpp —— 日志宏体系：KV_LOG_* + LogLevel + nowStr（叶子模块）
// ----------------------------------------------------------------------------
// 【设计思想·教程"KV_LOG 宏"节】
//   ① 为什么用宏不用函数：__FILE__/__LINE__/__func__ 这三个"调用点信息"
//      只在宏展开处才有效 —— 传给函数就只能拿到函数自己的位置（无意义）；
//   ② do-while(0) 包裹：让"宏调用"在语法上等价于一条语句，
//      if-else 悬挂分支不会错配（经典防御）；
//   ③ KV_LOG_LEVEL 条件编译：发布版一行 -DKV_LOG_LEVEL=2 就能裁掉
//      Debug/Info 日志的"判断 + 拼接"全部代码 —— 零运行时开销的开关。
// ============================================================================

enum class LogLevel { Debug, Info, Warn, Error };
// enum class 强作用域枚举：Debug/Info 不会污染外层命名空间，
// 且比较时必须显式 static_cast<int>（见宏里的用法）

inline const char* levelName(LogLevel l) noexcept {
    // 【设计细节】返回的字符串统一 5 字符宽（"INFO "/"WARN "带尾空格）——
    // 日志列对齐：不用 iomanip 也能肉眼扫描成列
    switch (l) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info:  return "INFO ";
        case LogLevel::Warn:  return "WARN ";
        case LogLevel::Error: return "ERROR";
    }
    return "?????";
    // switch 覆盖全部枚举值后仍需兜底 return：防"未来加级别忘了改这里"
    // 的编译告警/控制流漏洞 —— 防御性写法
}

inline std::string nowStr() {
    auto t  = std::chrono::system_clock::now();        // 系统时钟（日志显示用——
    auto tt = std::chrono::system_clock::to_time_t(t); // 真实墙钟，秒级精度足够）
    std::tm tm{};               // {} 值初始化清零：localtime_r 不清残留字段
#if defined(_WIN32)
    localtime_s(&tm, &tt);      // Windows 版（参数顺序与 POSIX 相反）
#else
    localtime_r(&tt, &tm);      // 线程安全版本
#endif
    // 【对比 localtime()】非 _r 版返回静态缓冲区 —— 多线程同时打日志会
    // 互相覆盖同一块静态内存；_r 版写进调用方栈上的 tm —— 天然隔离
    std::ostringstream oss;
    oss << std::put_time(&tm, "%H:%M:%S");     // 时:分:秒（本地时区）
    return oss.str();
}

}  // namespace mkv

// 关键：宏要写在 namespace 外
// 【为什么】宏是预处理器层面的文本替换，不认识 C++ namespace ——
// 若写进 namespace，展开处可能不在该 namespace 里，::mkv::nowStr()
// 全局限定的写法已保证展开后任何位置都能找到符号（宏与命名空间解耦）
#ifndef KV_LOG_LEVEL
#define KV_LOG_LEVEL 1                  // 默认 Info 起步
// 0=Debug 1=Info 2=Warn 3=Error：数值越大越沉默 ——
// 编译时 -DKV_LOG_LEVEL=3 即可只留错误（发布版姿势）
#endif

#define KV_LOG_IMPL(level, lvname, ...)                                     \
    do {                                                                    \
        if (static_cast<int>(level) >= KV_LOG_LEVEL) {                      \
            /* 低于门槛的级别整体被编译器裁掉（常量条件分支消除）*/          \
            std::cerr << "[" << ::mkv::nowStr() << "] "                     \
                      << "[" << lvname << "] "                              \
                      << "[" << __FILE__ << ":" << __LINE__ << " "          \
                      << __func__ << "] "                                   \
                      /* __FILE__/__LINE__/__func__ 是三个预定义宏：      \
                         只在"展开点"取值 —— 这正是日志必须用宏的根本原因 */\
                      << __VA_ARGS__ << std::endl;                          \
            /* __VA_ARGS__：可变参数宏（C++20 前的编译器扩展转正），      \
               KV_LOG_INFO("a" << x) 的 "a" << x 整体填进 << 链           \
               —— 参数以"流表达式"形态传入，函数模板做不到这种惰性拼接 */ \
        }                                                                   \
    } while (0)
    // 【do-while(0) 的防御】展开成：
    //   if (cond) KV_LOG_INFO(x); else foo();
    // 若宏是裸 if，会与外层 else 错配（else 归了宏里的 if）——
    // do{}while(0) 是"一条语句"的语法等价物，彻底免疫悬挂问题。

#define KV_LOG_DEBUG(...) KV_LOG_IMPL(::mkv::LogLevel::Debug, "DEBUG", __VA_ARGS__)
#define KV_LOG_INFO(...)  KV_LOG_IMPL(::mkv::LogLevel::Info,  "INFO ", __VA_ARGS__)
#define KV_LOG_WARN(...)  KV_LOG_IMPL(::mkv::LogLevel::Warn,  "WARN ", __VA_ARGS__)
#define KV_LOG_ERROR(...) KV_LOG_IMPL(::mkv::LogLevel::Error, "ERROR", __VA_ARGS__)
// 四个级别入口：预填级别枚举与对齐后的名字文本 —— 调用方只写消息本体
