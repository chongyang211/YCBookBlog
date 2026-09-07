#include "mkv/aof.hpp"

#include <fstream>
#include <string>
#include <iostream>

#include "mkv/errors.hpp"    // IoError / AofCorrupted
#include "mkv/store.hpp"     // 需要 Store 完整定义才能调 store.set 等
#include "mkv/command.hpp"   // tokenize / makeCommand / Command
#include "mkv/log.hpp"       // KV_LOG_WARN

namespace mkv {

// ─── 构造函数：打开文件 + 建父目录 ────────────────
AofWriter::AofWriter(const fs::path& path) : path_(path) {
    // Step 1：递归创建父目录（存在则跳过）
    if (path.has_parent_path()) {
        std::error_code ec;
        fs::create_directories(path.parent_path(), ec);   // 不抛异常版
        if (ec) throw IoError("create dir failed: " + ec.message());
    }

    // Step 2：以"追加 + 二进制"模式打开
    //   app    = 每次 write 前自动 seek 到文件末尾
    //   binary = Windows 上不把 \n 偷偷转成 \r\n
    out_.open(path, std::ios::out | std::ios::app | std::ios::binary);
    if (!out_) throw IoError("open AOF failed: " + path.string());
}

// ─── append：写一条记录 ──────────────────────────
void AofWriter::append(std::string_view line) {
    if (line.empty()) return;
    out_.write(line.data(), static_cast<std::streamsize>(line.size()));
    if (!out_) throw IoError("write AOF failed");
}

// ─── flush：把内核 buffer 里的字节交给 OS ────────
// 注意 flush ≠ fsync：flush 只把用户态 buffer 交给内核，掉电还是可能丢。
void AofWriter::flush() { out_.flush(); }

// ─── replayAof：启动重放还原状态 ─────────────────
// 注意：签名和头文件保持一致，但**不重复写 = true 默认参数**
ReplayReport replayAof(const fs::path& path, Store& store, bool tolerant) {
    ReplayReport r{};                       // 值初始化，3 个字段全为 0
    if (!fs::exists(path)) return r;        // 全新启动（文件不存在）不是错误

    std::ifstream in(path);
    if (!in) throw IoError("open AOF for replay failed");

    std::string line;
    std::size_t lineNo = 0;
    while (std::getline(in, line)) {
        ++lineNo;
        if (line.empty()) continue;
        ++r.totalLines;

        try {
            auto tokens = tokenize(line);
            auto cmd    = makeCommand(tokens);
            if (cmd->isWrite()) {
                (void)cmd->execute(store);   // 重放时丢弃返回值
                ++r.replayed;
            }
        } catch (const KvError& e) {
            if (tolerant) {
                // 宽松：记日志、计数、继续下一行
                ++r.corrupted;
                KV_LOG_WARN("AOF replay corrupted at line " << lineNo << ": " << e.what());
            } else {
                // 严格：抛异常中止启动
                throw AofCorrupted(lineNo, e.what());
            }
        }
    }
    return r;
}

}  // namespace mkv
