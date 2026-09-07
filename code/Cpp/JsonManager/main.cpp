#include "KvDatabase.h"
#include "JsonParser.h"
#include <iostream>
#include <sstream>

using namespace jsonkv;

void repl(KvDatabase& db) {
    std::cout << "jsonkv REPL.\n"
                 "Commands:\n"
                 "  GET <path>          查询路径值\n"
                 "  SET <path> <json>   设置（json 是合法 JSON）\n"
                 "  DEL <path>          删除\n"
                 "  SAVE                立即落盘\n"
                 "  EXIT                退出（自动落盘）\n";

    std::string line;
    while (std::cout << "> ", std::getline(std::cin, line)) {
        std::istringstream iss(line);
        std::string cmd; iss >> cmd;

        try {                                     // 顶层一把抓所有 JsonError
            if (cmd == "GET") {
                std::string path; iss >> path;
                auto v = db.get(path);
                std::cout << (v ? *v : "(nil)") << "\n";
            }
            else if (cmd == "SET") {
                std::string path; iss >> path;
                std::string rest;
                std::getline(iss, rest);          // path 后面剩下的整行 = JSON
                JsonParser p(rest);
                db.set(path, p.parse());
                std::cout << "OK\n";
            }
            else if (cmd == "DEL") {
                std::string path; iss >> path;
                std::cout << (db.del(path) ? "OK" : "(nil)") << "\n";
            }
            else if (cmd == "SAVE") {
                db.save();
                std::cout << "saved\n";
            }
            else if (cmd == "EXIT") {
                break;
            }
            else if (cmd.empty()) {
                continue;
            }
            else {
                std::cout << "unknown command\n";
            }
        }
        catch (const JsonError& e) {              // 解析错/类型错/IO错都进这里
            std::cout << "ERR " << e.what() << "\n";
        }
    }
}

int main(int argc, char** argv) {
    KvDatabase db(argc > 1 ? argv[1] : "kv.json");
    db.load();
    repl(db);
    db.save();
    return 0;
}
