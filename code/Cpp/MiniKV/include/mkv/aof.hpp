#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <cstddef>

namespace fs = std::filesystem;   // 全局命名空间别名，全项目复用

namespace mkv {

class Store;   // 前置声明：replayAof 只用 Store&，无需完整定义

// AOF 追加写日志：RAII 管理文件句柄，追加写入不覆盖
class AofWriter {
public:
    explicit AofWriter(const fs::path& path);   // explicit 防止隐式转换

    void append(std::string_view line);   // 追加一条 AOF 记录（含 \n）
    void flush();                         // 显式冲刷（由 fsync 策略决定何时调）

    ~AofWriter() = default;               // RAII：ofstream 析构时自动关文件

    // 五法则：文件句柄不能拷贝
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
};

// 重放 AOF 文件，把每一行调用 makeCommand + execute 应用到 store
// tolerant=true（默认）：坏行跳过 + 记日志 + 继续  → 生产推荐
// tolerant=false        ：坏行抛 AofCorrupted     → 严格模式
ReplayReport replayAof(const fs::path& path, Store& store, bool tolerant = true);

}  // namespace mkv
