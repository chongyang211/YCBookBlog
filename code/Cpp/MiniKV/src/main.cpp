#include <iostream>
#include <string>

#include "mkv/store.hpp"
#include "mkv/aof.hpp"
#include "mkv/server.hpp"
#include "mkv/command.hpp"
#include "mkv/errors.hpp"
#include "mkv/log.hpp"

using namespace mkv;

int main() {
    try {
        fs::path aofPath = "data/aof.log";
        Store store;
        auto report = replayAof(aofPath, store);
        KV_LOG_INFO("replayed " << report.replayed << " keys, skipped "
                    << report.corrupted << " corrupted");

        AofWriter aof(aofPath);
        Server    server(store);   // 启动后台清理

        std::cout << "MiniKV v1.0 - type EXIT to quit." << std::endl;
        std::string line;
        while (true) {
            std::cout << "> " << std::flush;
            if (!std::getline(std::cin, line)) break;   // Ctrl-D 退出
            if (line.empty()) continue;

            try {
                auto tokens = tokenize(line);
                if (tokens.empty()) continue;

                auto type = parseCmdType(std::string(tokens[0]));

                // EXIT 单独处理，不走 makeCommand
                if (type == CmdType::Exit) {
                    std::cout << "bye." << std::endl;
                    break;
                }

                auto cmd = makeCommand(tokens);
                std::string out = cmd->execute(store);

                // SAVE：强制刷盘（本身不产生 AOF 记录）
                if (type == CmdType::Save) {
                    aof.flush();
                }

                // 写命令同步落盘，确保 ACK 时数据已在文件里
                if (cmd->isWrite()) {
                    aof.append(cmd->toAofLine());
                    aof.flush();
                }
                std::cout << out << std::endl;
            }
            catch (const KvError& e) {
                std::cout << "(error) " << e.what() << std::endl;
            }
        }
        // server 析构 → 后台线程自动停 → 然后 aof 析构 → store 析构
    }
    catch (const std::exception& e) {
        KV_LOG_ERROR("fatal: " << e.what());
        return 1;
    }
    return 0;
}
