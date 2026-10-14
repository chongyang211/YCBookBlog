// ============================================================================
// 【文件定位】main.cpp —— 入口：异常体系自测 + KV 数据库 REPL 交互壳
// ----------------------------------------------------------------------------
// 【设计思路】两段式启动：
//   ① test()：冒烟测试 —— 异常体系多态捕获 + JsonNode 赋值语义验证；
//   ② repl()：读-求值-打印循环 —— GET/SET/DEL/SAVE/EXIT 五命令，
//     顶层 try-catch 一把抓所有 JsonError，让"单条命令失败"不炸整个循环。
// ============================================================================

#include "KvDatabase.h"
#include "JsonParser.h"
#include <iostream>
#include <sstream>

using namespace jsonkv;

// REPL：Read-Eval-Print Loop（读一行 → 执行 → 打印结果 → 循环）
// 【设计要点】每条命令包在 try-catch 里：任何解析错/类型错只报 ERR 继续
//   下一条 —— 一个"错误局部化"的交互循环骨架（Lua/Redis CLI 同款思路）。
void repl(KvDatabase& db) {
    std::cout << "jsonkv REPL.\n"
                 "Commands:\n"
                 "  GET <path>          查询路径值\n"
                 "  SET <path> <json>   设置（json 是合法 JSON）\n"
                 "  DEL <path>          删除\n"
                 "  SAVE                立即落盘\n"
                 "  EXIT                退出（自动落盘）\n";

    std::string line;
    // 【循环条件设计】逗号表达式：先执行 "std::cout << "> ""（打印提示符），
    //   再取 getline 的返回值（流引用，读到 EOF 转为 false）—— 提示符与
    //   读取一体化，Ctrl+D（EOF）自然退出循环。
    while (std::cout << "> ", std::getline(std::cin, line)) {
        std::istringstream iss(line);        // 把整行包成流，便于按空格切词
        std::string cmd; iss >> cmd;          // 第一个词 = 命令名（>> 自动跳过前导空白）

        try {                                     // 顶层一把抓所有 JsonError
            if (cmd == "GET") {
                std::string path; iss >> path;
                auto v = db.get(path);            // optional<string>
                // 【optional 的消费姿势】operator bool 判有值，解引用 * 取内容 ——
                //   "可能没有"逼着调用方处理两种情况（编译器级别的防御）
                std::cout << (v ? *v : "(nil)") << "\n";   // 无值打印 (nil)（Redis 风格）
            }
            else if (cmd == "SET") {
                std::string path; iss >> path;
                std::string rest;
                std::getline(iss, rest);          // path 后面剩下的整行 = JSON
                // 【取整行技巧】iss 已读走 "SET path"，getline 从当前位置读到
                //   行尾 —— 拿到完整的 JSON 文本（JSON 里可能含空格，不能 >> 切）
                JsonParser p(rest);               // 解析 JSON 值（失败抛 JsonParseError）
                db.set(path, p.parse());          // 节点所有权（unique_ptr）移交给库
                std::cout << "OK\n";
            }
            else if (cmd == "DEL") {
                std::string path; iss >> path;
                std::cout << (db.del(path) ? "OK" : "(nil)") << "\n";   // 删没删着都有回音
            }
            else if (cmd == "SAVE") {
                db.save();                        // 手动落盘（怕崩溃丢数据就随手 SAVE）
                std::cout << "saved\n";
            }
            else if (cmd == "EXIT") {
                break;                            // 跳出 REPL 循环（main 里还会再 save 一次兜底）
            }
            else if (cmd.empty()) {
                continue;                         // 空行：静默跳过（不算未知命令）
            }
            else {
                std::cout << "unknown command\n";
            }
        }
        catch (const JsonError& e) {              // 解析错/类型错/IO错都进这里
            // 【多态捕获的价值】基类引用接住四种子异常，e.what() 虚派发到
            //   各自拼好的消息（带行列号/期望实际/键名）—— REPL 不用关心
            //   具体是哪种错，一句 ERR 统一回复，循环继续。
            std::cout << "ERR " << e.what() << "\n";
        }
    }
}

// demo：构造第 which 种异常（供 test() 逐一验证子类可被父类接住）
void demo(int which) {
    switch (which) {
        case 1: throw jsonkv::JsonParseError("bad token", 3, 12);      // 解析错（带行列）
        case 2: throw jsonkv::TypeMismatchError("string", "number");   // 类型错
        case 3: throw jsonkv::KeyNotFoundError("user.name");           // 键不存在
        case 4: throw jsonkv::JsonIoError("cannot open kv.json");      // IO 错
        default: throw jsonkv::JsonError("base class works!");         // 基类自己
    }
}

void test() {
    // ① 基类异常可以被 std::exception 家族接住（继承自 runtime_error 的红利：
    //    即使调用方根本不认识 JsonError，也不会直接 terminate）
    try {
        throw jsonkv::JsonError("base class works!");
    } catch (const std::exception& e) {     // 用基类捕获也行
        std::cout << "caught: " << e.what() << "\n";
    }

    // ② 四种子异常全部被父类 JsonError 一网打尽（多态捕获）
    for (int i = 1; i <= 5; ++i) {
        try { demo(i); }
        catch (const JsonError& e) {                     // ⭐ 父类一次接住
            std::cout << "[" << i << "] " << e.what() << "\n";
        }
    }
    // 【教学对照·explicit 的效果】下面这行是教程里的"反例现场"：
    //     jsonkv::JsonNode n; n = 3.14;        // ❌ 编译不过！
    //   原因：JsonNode 的构造函数全部 explicit —— 3.14 (double) 不能隐式
    //   转成 JsonNode，而赋值运算符只有"从 JsonNode 赋值"一种。
    //   正确写法是显式构造一个节点再移动赋值：
    jsonkv::JsonNode n;
    n = jsonkv::JsonNode{3.14};     // ✅ 显式构造临时节点 + 移动赋值（零拷贝）
    //   这正是 explicit 设计意图的实证：危险的双向隐式转换通道被关闭，
    //   想把 3.14 变节点？必须写花括号"明示"。
}

// g++ -std=c++17 main.cpp JsonNode.cpp JsonParser.cpp JsonWriter.cpp KvDatabase.cpp
// （命令行编译方式；CMakeLists.txt 是工程化方案）

// main：自测 → 开库（可命令行指定文件）→ 进 REPL → 退出兜底存盘
int main(int argc, char** argv) {
    test();     // 冒烟自测（异常体系 + 赋值语义），失败会直接抛出未捕获异常终止
    // 【数据文件选择】argv[1] 优先（./jsonkv my.json），否则默认 kv.json ——
    //   无第三方参数解析库时最朴素的命令行处理
    KvDatabase db(argc > 1 ? argv[1] : "kv.json");
    db.load();      // 冷启动恢复（文件不存在 → 空库，正常继续）
    repl(db);       // 交互循环（EXIT 或 EOF 退出）
    db.save();      // 退出后显式落盘（哪怕没改过也存一次 —— dirty=false 时也
                    // 无害，重复写幂等；析构函数的 dirty 自动补存是最后防线）
    return 0;
}
