// main.cpp —— MiniKV 主入口
//
// 启动顺序：
//   1. replayAof(aofPath, store)   ——— 从 AOF 还原数据
//   2. AofWriter aof(aofPath)      ——— 打开追加流
//   3. Server server(store)        ——— 启动后台 TTL 清理（jthread）
//
// 析构顺序（栈逆序，关键！）：
//   server -> aof -> store
//   先停后台线程，再关文件，最后释放数据，才不会出现"线程访问已销毁 store"的 UB。

#include "mkv/aof.hpp"
#include "mkv/command.hpp"
#include "mkv/errors.hpp"
#include "mkv/log.hpp"
#include "mkv/server.hpp"
#include "mkv/store.hpp"

#include <exception>
#include <filesystem>
#include <iostream>
#include <string>

namespace {

// 单条命令执行；写命令同时落 AOF。SAVE 命令额外 fsync。
void runOnce(const std::string& line, mkv::Store& store, mkv::AofWriter& aof) {
    auto tokens = mkv::tokenize(line);
    if (tokens.empty()) return;

    // EXIT 与 SAVE 由 main 兜底，不走 makeCommand
    auto type = mkv::parseCmdType(tokens[0]);
    if (type == mkv::CmdType::Save) {
        aof.flush();
        std::cout << "OK\n";
        return;
    }

    auto cmd = mkv::makeCommand(tokens);
    std::string out = cmd->execute(store);
    if (cmd->isWrite()) {
        aof.append(cmd->toAofLine());
        aof.flush();   // 每次写都 flush，进程被 kill -9 也不丢
    }
    std::cout << out << "\n";
}

}  // namespace

int main() {
    try {
        const std::filesystem::path aofPath = "data/aof.log";

        mkv::Store store;
        mkv::replayAof(aofPath, store);
        KV_LOG_INFO("replayed " << store.size() << " keys");

        mkv::AofWriter aof(aofPath);
        mkv::Server    server(store);   // 启动后台 TTL 清理

        std::cout << "MiniKV v1.0 — type EXIT to quit.\n";

        std::string line;
        while (true) {
            std::cout << "> " << std::flush;
            if (!std::getline(std::cin, line)) break;   // Ctrl-D 退出
            if (line.empty()) continue;

            // EXIT 单独处理，绕过 makeCommand
            auto first = mkv::tokenize(line);
            if (!first.empty() &&
                mkv::parseCmdType(first[0]) == mkv::CmdType::Exit) {
                std::cout << "bye.\n";
                break;
            }

            try {
                runOnce(line, store, aof);
            }
            catch (const mkv::CmdSyntaxError& e) {
                std::cout << "(syntax) " << e.what() << "\n";
            }
            catch (const mkv::TypeError& e) {
                std::cout << "(type) " << e.what() << "\n";
            }
            catch (const mkv::IoError& e) {
                // IO 错误较严重，记 cerr 但不退出
                std::cerr << "(io) " << e.what() << "\n";
            }
            catch (const mkv::KvError& e) {
                std::cerr << "(internal) " << e.what() << "\n";
            }
            catch (const std::exception& e) {
                std::cerr << "(unexpected) " << e.what() << "\n";
            }
        }
        // 析构顺序：server -> aof -> store （栈上声明逆序）
    }
    catch (const mkv::AofCorrupted& e) {
        std::cerr << "FATAL: " << e.what() << "\n";
        return 2;
    }
    catch (const mkv::IoError& e) {
        std::cerr << "FATAL IO: " << e.what() << "\n";
        return 3;
    }
    catch (const std::exception& e) {
        std::cerr << "FATAL: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
