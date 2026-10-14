#include "mkv/aof.hpp"

#include <fstream>     // ifstream（replay 读文件）
#include <string>      // std::string / getline
#include <iostream>    // （KV_LOG 宏内部用到的 cerr 已由 log.hpp 引入）

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
        // 【ec 版 vs 抛异常版】filesystem 的 API 成对提供：
        //   抛异常版直接 throw；ec 版把错误码写进 out 参数 ——
        //   "目录已存在"在这类调用中是常态而非错误，用 ec 版自行甄别
        if (ec) throw IoError("create dir failed: " + ec.message());
    }

    // Step 2：以"追加 + 二进制"模式打开
    //   app    = 每次 write 前自动 seek 到文件末尾
    //   binary = Windows 上不把 \n 偷偷转成 \r\n
    out_.open(path, std::ios::out | std::ios::app | std::ios::binary);
    // 【app 的深意】AOF 的历史不可覆盖（见头文件设计三问）—— app 模式
    //   保证每次 write 都落在文件尾，即使别处重定位也无效；
    //   【binary 的跨平台性】文本模式下 Windows 把 \n 翻译成 \r\n 落盘，
    //   重放时 getline 拿到带 \r 的行 —— 二进制模式让字节原样进原样出
    if (!out_) throw IoError("open AOF failed: " + path.string());
    // 构造失败即抛：调用方拿到的 AofWriter 要么可写要么不存在（无半成品）
}

// ─── append：写一条记录 ──────────────────────────
void AofWriter::append(std::string_view line) {
    if (line.empty()) return;      // 空行防御：不产生垃圾字节
    out_.write(line.data(), static_cast<std::streamsize>(line.size()));
    // 【write 而非 <<】operator<< 遇 '\0' 截断、遇空格无碍但慢 ——
    //   write 直写"data 起 size 字节"，字节级精确（view 没有 c_str()，
    //   也不能保证 NUL 结尾 —— write 是唯一正确姿势）
    if (!out_) throw IoError("write AOF failed");
    // 写完检查流状态：磁盘满/权限变化等在下一刻才暴露 —— 早发现早抛
}

// ─── flush：把内核 buffer 里的字节交给 OS ────────
// 注意 flush ≠ fsync：flush 只把用户态 buffer 交给内核，掉电还是可能丢。
void AofWriter::flush() { out_.flush(); }
// 【持久化深水区】三层缓冲：用户态 ofstream buffer →（flush）→
//   内核 page cache →（fsync）→ 磁盘。教学版 flush（每命令一次，
//   崩溃丢"内核里未落盘的部分"）；生产版需 fsync（Redis 的
//   appendfsync always/everysec/no 对应三档取舍）

// ─── replayAof：启动重放还原状态 ─────────────────
// 注意：签名和头文件保持一致，但**不重复写 = true 默认参数**
// 【C++ 语法规则】默认参数只能出现在声明处 —— 定义处重复写会编译错
ReplayReport replayAof(const fs::path& path, Store& store, bool tolerant) {
    ReplayReport r{};                       // 值初始化，3 个字段全为 0
    if (!fs::exists(path)) return r;        // 全新启动（文件不存在）不是错误
    // 【宽容的空态】首次运行无 AOF 是常态 —— 返回全零报表（而非异常），
    //   "从零开始的空库"与"坏文件"语义严格分开

    std::ifstream in(path);
    if (!in) throw IoError("open AOF for replay failed");
    // 存在却打不开（权限等）才是真错误 —— 抛 IoError 交给上层决策

    std::string line;
    std::size_t lineNo = 0;
    while (std::getline(in, line)) {        // 逐行读取（丢弃行尾 \n）
        ++lineNo;
        if (line.empty()) continue;         // 空行不算坏行也不算总行
        ++r.totalLines;

        // 【复用命令管道的精髓】重放一行 = "对 store 重新执行那条命令"：
        //   tokenize → makeCommand → execute —— 与 REPL 主循环走完全相同的
        //   代码路径。"恢复逻辑"不需要单独实现，历史命令自己会重建状态
        try {
            auto tokens = tokenize(line);
            auto cmd    = makeCommand(tokens);
            if (cmd->isWrite()) {
                (void)cmd->execute(store);   // 重放时丢弃返回值
                // 只重演写命令（读命令重放无意义）—— isWrite 虚函数的用武之地
                ++r.replayed;
            }
        } catch (const KvError& e) {
            // 【坏行的两种处置】教程§08 Step 8.5"手改 aof.log 制造截断"的
            //   造 BUG 现场：半行/缺参数/未知命令都在这里被接住
            if (tolerant) {
                // 宽松：记日志、计数、继续下一行
                ++r.corrupted;
                KV_LOG_WARN("AOF replay corrupted at line " << lineNo << ": " << e.what());
                // 坏行带行号告警：运维凭它直接定位到 aof.log 第几行
            } else {
                // 严格：抛异常中止启动
                throw AofCorrupted(lineNo, e.what());
                // 数据完整性优先模式：一行坏就拒绝服务，等人来修
            }
        }
        // 【只 catch KvError 的考量】命令层已知错误（语法/类型）才可跳过；
        //   未知异常（bad_alloc 等）继续外抛 —— 宽松不等于糊涂
    }
    return r;
}

}  // namespace mkv
