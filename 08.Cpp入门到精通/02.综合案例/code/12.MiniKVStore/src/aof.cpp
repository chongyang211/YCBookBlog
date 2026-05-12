// aof.cpp —— AOF 持久化实现
//
// 关键点：
// - AofWriter 构造函数失败抛 IoError，禁止"半初始化"对象；
// - replayAof 内部不写 AOF（不持有 AofWriter），物理上避免日志翻倍。

#include "mkv/aof.hpp"
#include "mkv/command.hpp"
#include "mkv/errors.hpp"
#include "mkv/store.hpp"

#include <fstream>
#include <string>
#include <system_error>

namespace mkv {

AofWriter::AofWriter(const fs::path& path) : path_(path) {
    if (path.has_parent_path()) {
        std::error_code ec;
        fs::create_directories(path.parent_path(), ec);
        if (ec) throw IoError("create dir failed: " + ec.message());
    }
    out_.open(path, std::ios::out | std::ios::app | std::ios::binary);
    if (!out_) throw IoError("open AOF failed: " + path.string());
}

void AofWriter::append(std::string_view line) {
    if (line.empty()) return;
    out_.write(line.data(), static_cast<std::streamsize>(line.size()));
    if (!out_) throw IoError("write AOF failed");
}

void AofWriter::flush() {
    out_.flush();
    if (!out_) throw IoError("flush AOF failed");
}

void replayAof(const fs::path& path, Store& store) {
    if (!fs::exists(path)) return;       // 全新启动，无 AOF

    std::ifstream in(path);
    if (!in) throw IoError("open AOF for replay failed");

    std::string line;
    std::size_t lineNo = 0;
    while (std::getline(in, line)) {
        ++lineNo;
        if (line.empty()) continue;

        try {
            auto tokens = tokenize(line);
            if (tokens.empty()) continue;
            auto cmd = makeCommand(tokens);
            if (!cmd->isWrite()) continue;       // 只读命令不应出现在 AOF
            (void)cmd->execute(store);            // 重放时丢弃返回值
        } catch (const KvError& e) {
            // 严格策略：一行损坏直接抛出，由 main 决定是否退出
            throw AofCorrupted(lineNo, e.what());
        }
    }
}

}  // namespace mkv
