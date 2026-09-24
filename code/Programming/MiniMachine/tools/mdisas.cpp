#include "mm/instruction.hpp"
#include <cstdio>
#include <fstream>
#include <vector>

int main(int argc, char** argv) {
    if (argc < 2) { std::fprintf(stderr, "usage: mdisas <input.bin>\n"); return 1; }
    std::ifstream in(argv[1], std::ios::binary);
    if (!in) { std::fprintf(stderr, "open %s failed\n", argv[1]); return 2; }

    // 读整个文件
    std::vector<uint32_t> words;
    uint32_t w;
    while (in.read(reinterpret_cast<char*>(&w), 4)) words.push_back(w);

    uint32_t pc = 0;
    for (auto raw : words) {
        auto inst = mm::decode(raw);
        std::printf("0x%08X:  %08X   %s\n", pc, raw, mm::disasm(inst).c_str());
        pc += 4;
    }
    return 0;
}
