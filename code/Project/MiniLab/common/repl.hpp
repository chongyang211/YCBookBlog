// common/repl.hpp — 12 盒共用的 REPL 骨架
#pragma once
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace mlab {

class Repl {
public:
    using Cmd = std::function<void(const std::vector<std::string>&)>;

    void reg(std::string name, std::string help, Cmd f) {
        order_.push_back(name);
        cmds_[name] = {std::move(help), std::move(f)};
    }

    int run(std::string_view prompt = ">>> ") {
        std::string line;
        while (std::cout << prompt && std::getline(std::cin, line)) {
            if (line.empty()) continue;
            auto args = split(line);
            if (args.empty()) continue;
            auto& head = args[0];
            if (head == "quit" || head == "exit") return 0;
            if (head == "help") { show_help(); continue; }
            auto it = cmds_.find(head);
            if (it == cmds_.end()) {
                std::cout << "unknown: " << head << " (try `help`)\n";
                continue;
            }
            try {
                it->second.fn({args.begin() + 1, args.end()});
            } catch (const std::exception& e) {
                std::cout << "err: " << e.what() << "\n";
            }
        }
        return 0;
    }

private:
    struct Entry { std::string help; Cmd fn; };

    static std::vector<std::string> split(const std::string& s) {
        std::vector<std::string> r;
        std::stringstream ss(s);
        std::string tok;
        while (ss >> tok) r.push_back(tok);
        return r;
    }

    void show_help() {
        std::cout << "commands:\n";
        for (auto& k : order_)
            std::cout << "  " << k << "  - " << cmds_[k].help << "\n";
        std::cout << "  help / quit\n";
    }

    std::unordered_map<std::string, Entry> cmds_;
    std::vector<std::string> order_;
};

}  // namespace mlab
