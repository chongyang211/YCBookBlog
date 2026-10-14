#pragma once

#include <iostream>     // std::cout
#include <mutex>        // std::mutex / lock_guard
#include <sstream>      // std::ostringstream
#include <thread>       // std::this_thread::get_id
#include <chrono>       // 时钟：system_clock / steady_clock
#include <iomanip>      // std::put_time / setw / setfill
#include <ctime>        // std::tm / localtime_r

// ============================================================================
// 【组件定位】Logger —— 线程安全日志：整行原子输出（教程阶段⑥）
// ----------------------------------------------------------------------------
// 【设计思路·演进史】
//   Step 6.1 裸 cout 多线程：日志字符级撕裂乱序（"用户 1 提交"和
//     "用户 2 成功"的字节交错混排）—— cout 的 << 每次调用是独立的，
//     多线程并发插入时输出流没有行概念；
//   Step 6.2 mutex + 折叠表达式：把"整行"的多个 << 全部圈进一个锁内 ——
//     行级原子性（一行内容不会被另一行插进来撕开）。
// 【两个技术点】
//   static inline mutex（C++17）：头文件里直接定义静态成员，免类外定义；
//   (cout << ... << args)：C++17 折叠表达式，把参数包逐个 << 进流。
// ============================================================================

// 线程安全日志：mutex 保证整行原子输出，C++17 折叠表达式拼接参数
class Logger {
private:
    static inline std::mutex mtx;        // C++17 inline static：头文件里就能初始化
    // 【C++17 之前】static 成员必须在某个 .cpp 里定义一次（否则链接错），
    // header-only 库没有 .cpp —— inline 变量破了死局：每个包含它的
    // 翻译单元合并为同一实体，头文件里直接定义合法。

    // 生成 "HH:MM:SS.mmm" 格式时间戳
    static std::string timestamp() {
        auto now = std::chrono::system_clock::now();       // 系统时钟（可回拨，仅用于显示）
        auto t   = std::chrono::system_clock::to_time_t(now);  // 转 Unix 秒
        auto ms  = std::chrono::duration_cast<std::chrono::milliseconds>(
                      now.time_since_epoch()) % 1000;      // 纪元毫秒取余 → 毫秒位
        std::tm local{};
        // tm 结构体先清零：localtime_r 只填字段不清残值 —— 值初始化 {} 保证干净
#if defined(_WIN32)
        localtime_s(&local, &t);           // Windows 版（参数顺序与 POSIX 相反）
#else
        localtime_r(&t, &local);         // 线程安全版本
        // 【对比 localtime()】非 _r 版返回静态缓冲区指针 —— 两个线程
        // 同时格式化时间会互相覆盖同一块静态内存（数据竞争）；
        // _r 版把结果写进调用方的 local —— 调用方栈上各写各的，天然隔离
#endif
        std::ostringstream oss;
        oss << std::put_time(&local, "%H:%M:%S")           // 时:分:秒（本地时区）
            << "." << std::setw(3) << std::setfill('0') << ms.count();
        // setw(3)+setfill('0')：毫秒补零对齐（7ms 显示 "007" 而非 "7"）；
        // setw 只作用于下一个输出项（一次性），setfill 是粘性的
        return oss.str();
    }

public:
    template <typename... Args>
    static void info(Args&&... args) {
        std::lock_guard<std::mutex> lock(mtx);                       // 整行原子
        // 锁的粒度 = 整行（时间戳 + 线程号 + 全部参数 + 换行）：
        // 撕裂的根源是"一行的多次 << 被其他行插队"，圈进一个锁就无懈可击
        std::cout << "[" << timestamp() << "][T"
                  << std::this_thread::get_id() << "] ";
        // 线程 id 让日志可按线程归组 —— 并发日志的必备上下文
        (std::cout << ... << std::forward<Args>(args));              // C++17 折叠表达式
        // 【折叠表达式】(pack op ...) 是"左折叠"：等价展开为
        //   cout << arg1 << arg2 << ... << argN
        // —— 变参日志不用递归展开，一行搞定（C++17 之前要写
        //   递归重载或初始化列表技巧）
        std::cout << "\n";
    }
};
