// ============================================================================
// 【文件定位】main.cpp —— REPL 主入口（组装层：把 store/aof/server/command 串起来）
// ----------------------------------------------------------------------------
// 【设计思路】main 只做"接线"：
//   启动序列（重放 AOF → 开 AofWriter → 起 Server）→ REPL 循环
//   （tokenize → makeCommand → execute → 写命令落盘）→ 退出时析构链收尾。
// 双层 try 的分工：
//   外层 catch std::exception —— 启动失败（AOF 打不开等）致命退出；
//   内层 catch KvError —— 单条命令失败只报 (error)，循环继续。
// ============================================================================

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
        // 相对路径：相对"进程工作目录"（从 MiniKV/ 目录启动时落在其 data/ 下）
        Store store;
        // 栈上 Store：先有仓库，后面的重放/命令才有落点
        auto report = replayAof(aofPath, store);
        // 【冷启动恢复】重放历史命令重建内存状态 —— AOF 承诺的兑现现场
        KV_LOG_INFO("replayed " << report.replayed << " keys, skipped "
                    << report.corrupted << " corrupted");
        // 报表日志：恢复了多少、跳过多少坏行（重放透明化的关键一环）

        AofWriter aof(aofPath);
        // 重放完才开写句柄：若先开 app 模式句柄再重放……（顺序无冲突，
        // 但"先恢复后记录"的叙事顺序让启动流程因果清晰）
        Server    server(store);   // 启动后台清理
        // jthread 构造即起线程 —— Server 一落地，purgeExpired 每 100ms 跑

        std::cout << "MiniKV v1.0 - type EXIT to quit." << std::endl;
        std::string line;
        while (true) {
            std::cout << "> " << std::flush;
            // flush：提示符立即上屏（cout 行缓冲，不冲刷会憋到行尾）
            if (!std::getline(std::cin, line)) break;   // Ctrl-D 退出
            // getline 返回流引用，EOF/错误转 bool 为 false —— 优雅退出
            if (line.empty()) continue;     // 空行静默跳过（不打错误）

            try {
                auto tokens = tokenize(line);
                if (tokens.empty()) continue;    // 全空白行：tokenize 产物为空

                auto type = parseCmdType(std::string(tokens[0]));
                // 先解析类型：EXIT 要在进工厂前拦截（见下）

                // EXIT 单独处理，不走 makeCommand
                if (type == CmdType::Exit) {
                    std::cout << "bye." << std::endl;
                    break;
                    // 【为什么 EXIT 不做成命令】EXIT 是"控制 REPL 生命周期"
                    //   的元操作，不是数据操作 —— 若做成 Command 还得给它
                    //   execute 返回什么、isWrite 是 false、AOF 里记不记……
                    //   全是拧巴的语义。元控制留在主循环，命令留给数据操作
                }

                auto cmd = makeCommand(tokens);
                std::string out = cmd->execute(store);
                // 多态调用：cmd 指向哪个派生类就执行哪个 execute ——
                // 命令模式的运行时分发，主循环对具体命令一无所知

                // SAVE：强制刷盘（本身不产生 AOF 记录）
                if (type == CmdType::Save) {
                    aof.flush();
                    // flush 兜底：把内核 buffer 里所有积压字节交给 OS
                    //（每条写命令本来就即时 flush —— SAVE 更多是
                    // 给用户一个"确保落盘"的心理锚点与手动触发口）
                }

                // 写命令同步落盘，确保 ACK 时数据已在文件里
                if (cmd->isWrite()) {
                    aof.append(cmd->toAofLine());
                    // toAofLine 用原始 token 拼行（不是格式化结果）——
                    // 保证"回放 == 当时的命令"
                    aof.flush();
                    // 每写必冲：牺牲吞吐换"回复 OK 时数据已在文件"的强承诺
                    //（Redis appendfsync everysec 是吞吐与安全的折中版）
                }
                std::cout << out << std::endl;
            }
            catch (const KvError& e) {
                std::cout << "(error) " << e.what() << std::endl;
                // 单条命令失败：报错继续下一轮 —— 错误局部化（REPL 的生命线）
            }
        }
        // server 析构 → 后台线程自动停 → 然后 aof 析构 → store 析构
        // 【析构链的顺序即声明逆序】server 先死（jthread request_stop+join，
        //   purger 安静退出）→ aof 关文件（最后的缓冲落盘）→ store 释放
        //   —— "先停线程、再关文件、最后放数据"的顺序无懈可击：
        //   purger 若比 store 晚死会访问已析构的 store（悬垂引用）！
    }
    catch (const std::exception& e) {
        KV_LOG_ERROR("fatal: " << e.what());
        // 外层兜底：启动阶段（重放/开文件）的失败是致命的 —— 日志 + 退出码 1
        return 1;
    }
    return 0;
}
