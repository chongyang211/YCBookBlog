#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <vector>

#include "mkv/command.hpp"
#include "mkv/value.hpp"
using namespace mkv;

// strategy_installer_demo.cpp
// 编译运行：g++ -std=c++17 strategy_installer_demo.cpp -o demo && ./demo
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <future>
#include <thread>



void test() {

}

int main() {
    test();
    return 0;
    std::cout << "MiniKV v0.1 - type EXIT to quit.\n";
    std::string line;
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) {
            // Ctrl-D 退出
            break;
        }
        if (line.empty()) {
            continue;
        }

        // 切词
        std::istringstream iss(line);
        std::vector<std::string> tokens;
        for (std::string tok; iss >> tok; ) {
            tokens.push_back(tok);
        }
        if (tokens.empty()) {
            continue;
        }
        switch (parseCmdType(tokens[0])) {
            case CmdType::Set:     std::cout << "[TODO] SET\n";    break;
            case CmdType::Get:     std::cout << "[TODO] GET\n";    break;
            case CmdType::Del:     std::cout << "[TODO] DEL\n";    break;
            case CmdType::Expire:  std::cout << "[TODO] EXPIRE\n"; break;
            case CmdType::Ttl:     std::cout << "[TODO] TTL\n";    break;
            case CmdType::Keys:    std::cout << "[TODO] KEYS\n";   break;
            case CmdType::Save:    std::cout << "[TODO] SAVE\n";   break;
            case CmdType::Exit:    std::cout << "bye.\n";          return 0;
            case CmdType::Unknown: std::cout << "(error) unknown command\n"; break;
        }

        std::cout << "(unknown) you said: ";
        for (auto& t : tokens) {
            std::cout << "[" << t << "] ";
        }
        std::cout << "\n";
    }
    return 0;
}
