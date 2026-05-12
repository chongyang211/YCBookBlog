// mkv/aof.hpp —— AOF (Append-Only File) 持久化
//
// 关键设计（详见 12.迷你KV存储引擎.md §08）：
// - AofWriter 遵守 RAII：构造打开（失败抛 IoError），析构关闭；
// - std::ios::app + std::ios::binary：跨平台安全追加；
// - replayAof：启动时一行行 tokenize+makeCommand+execute，
//   重放过程中"绕过 AofWriter"避免日志翻倍。

#pragma once

#include <filesystem>
#include <fstream>
#include <string_view>

namespace mkv {

class Store;

namespace fs = std::filesystem;

class AofWriter {
public:
    explicit AofWriter(const fs::path& path);

    // 移动允许，拷贝禁止（fstream 也是这样）
    AofWriter(const AofWriter&)            = delete;
    AofWriter& operator=(const AofWriter&) = delete;
    AofWriter(AofWriter&&)                 = default;
    AofWriter& operator=(AofWriter&&)      = default;

    void append(std::string_view line);
    void flush();

    ~AofWriter() = default;

private:
    fs::path      path_;
    std::ofstream out_;
};

// 启动时把 AOF 文件回放到 store；不存在则什么都不做
void replayAof(const fs::path& path, Store& store);

}  // namespace mkv
