#include "mm/machine.hpp"
#include "mm/log.hpp"
#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>

int main(int argc, char** argv) {
    // 命令行直跑模式: mmach [--trace] <path.bin>
    bool wantTrace = false;
    std::string binPath;
    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "--trace") wantTrace = true;
        else                binPath = a;
    }

    mm::Machine machine;
    if (!binPath.empty()) {
        machine.cmdLoad(binPath);
        if (wantTrace) machine.cpu().enableTrace(true);
        machine.cmdRun();
        machine.cmdDump();
        return 0;
    }

    // 交互式 REPL
    std::string line;
    std::puts("MiniMachine v1.0  (Ctrl-D or `quit` to exit)");
    while (true) {
        std::cout << ">>> " << std::flush;
        if (!std::getline(std::cin, line)) break;

        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;
        if (cmd.empty()) continue;

        if      (cmd == "dump")  machine.cmdDump();
        else if (cmd == "run")   machine.cmdRun();
        else if (cmd == "step")  machine.cmdStep();
        else if (cmd == "regs") {
            for (int i = 0; i < 8; ++i)
                std::printf("R%d=0x%08X(%d)%s", i, machine.cpu().r(i),
                    (int)machine.cpu().r(i), (i % 4 == 3) ? "\n" : "  ");
        }
        else if (cmd == "trace") {
            std::string s; iss >> s;
            machine.cpu().enableTrace(s != "off");
            std::printf("[main] trace %s\n", (s != "off") ? "ON" : "OFF");
        }
        else if (cmd == "load") { std::string p; iss >> p; machine.cmdLoad(p); }
        else if (cmd == "quit" || cmd == "exit") break;
        else if (cmd == "help")
            std::puts("commands: load <path.bin> | run | step | regs | trace on|off | dump | quit");
        else MM_ERR("repl", "unknown command: %s (try 'help')", cmd.c_str());
    }
    std::puts("bye.");
    return 0;
}
