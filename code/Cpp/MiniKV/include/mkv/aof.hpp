#pragma once

#include <filesystem>     // std::filesystem::path / create_directories
#include <fstream>        // std::ofstream / ifstream
#include <string>         // std::string
#include <string_view>    // std::string_view（append 参数）
#include <cstddef>        // std::size_t

namespace fs = std::filesystem;   // 全局命名空间别名，全项目复用
// 【为什么放全局】aof.hpp 被 main.cpp 等处包含后，"fs::path" 直接可用 ——
// 项目内统一的简称约定（教程目录结构一节明示）

namespace mkv {

// ============================================================================
// 【模块定位】aof.hpp —— AOF 持久化：AofWriter（追加写）+ replayAof（重放）
// ----------------------------------------------------------------------------
// 【设计思想·教程§08 灵魂三问】
//   ① 为什么 AOF 不用 dump（全量快照）？追加日志实现简单、天然增量、
//     崩溃恢复粒度细（最后一笔之前的都不丢）；dump 要"整个序列化 + 原子替换"
//     的复杂度（写临时文件 + rename），教学版选轻者；
//   ② 为什么追加不覆盖？AOF 的核心承诺是"历史不灭" —— 覆盖写一旦中途
//     崩溃，旧数据已毁新数据未全，两头落空；追加写最坏丢"半行"，
//     宽松模式跳过即可；
//   ③ flush ≠ fsync：flush 只把用户态缓冲交给内核，掉电仍可能丢 ——
//     教学版用 flush（性能好），生产版每笔/每秒 fsync（见 flush 注释）。
// ============================================================================

class Store;   // 前置声明：replayAof 只用 Store&，无需完整定义
// 头文件只需引用声明 —— 完整定义在 aof.cpp 里 include（保持头轻量 + 无循环）

// AOF 追加写日志：RAII 管理文件句柄，追加写入不覆盖
class AofWriter {
public:
    explicit AofWriter(const fs::path& path);   // explicit 防止隐式转换
    // 构造即"建目录 + 开文件"（失败抛 IoError）—— 强异常安全：对象
    // 要么"已开好可写"，要么构造失败不存在半成品

    void append(std::string_view line);   // 追加一条 AOF 记录（含 \n）
    // 参数 string_view：调用方传 cmd->toAofLine() 的 string 或字面量均可，
    //   view 避免一次多余拷贝（write 直捣底层字节）
    void flush();                         // 显式冲刷（由 fsync 策略决定何时调）

    ~AofWriter() = default;               // RAII：ofstream 析构时自动关文件
    // 显式 = default 表达"析构有意交给成员 RAII"（ofstream 自身的析构关句柄）

    // 五法则：文件句柄不能拷贝
    // 【为什么禁拷贝】两个 AofWriter 拷贝后指向同一句柄，析构两次 = UB；
    //   且 ofstream 本身不可拷贝 —— = delete 把事故堵在编译期
    AofWriter(const AofWriter&)            = delete;
    AofWriter& operator=(const AofWriter&) = delete;

private:
    fs::path      path_;    // 保留路径便于错误信息展示
    std::ofstream out_;     // 实际的写文件句柄
};

// 重放结果的统计信息
struct ReplayReport {
    std::size_t replayed   = 0;   // 成功重放的行数
    std::size_t corrupted  = 0;   // 跳过的坏行数
    std::size_t totalLines = 0;   // 非空总行数
    // 【为什么返回统计】启动日志要向用户交代"恢复了什么、跳过了什么" ——
    //   重放是黑盒过程，报表让黑盒透明（调试坏行问题的关键证据）
};

// 重放 AOF 文件，把每一行调用 makeCommand + execute 应用到 store
// tolerant=true（默认）：坏行跳过 + 记日志 + 继续  → 生产推荐
// tolerant=false        ：坏行抛 AofCorrupted     → 严格模式
// 【两种策略的取舍】宽松：个别坏行不挡服务启动（可用性优先）；
//   严格：数据完整性优先（坏一行就拒绝启动，人来看）——
//   Redis 的 aof-load-truncated 配置同款设计思路
ReplayReport replayAof(const fs::path& path, Store& store, bool tolerant = true);
// 默认参数只在声明写一次（定义处不重复）—— C++ 语法规则

}  // namespace mkv
