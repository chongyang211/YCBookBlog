#include "mm/instruction.hpp"
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace mm;

// ---------- 小工具:字符串处理 ----------
static void trim(std::string& s) {
    auto not_space = [](int c){ return !std::isspace(c); };
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), not_space));
    s.erase(std::find_if(s.rbegin(), s.rend(), not_space).base(), s.end());
}
static std::string stripComment(const std::string& line) {
    auto p = line.find(';');
    return (p == std::string::npos) ? line : line.substr(0, p);
}
// 把 "R3" / "#5" / "[R2+4]" 这种解析成结构化数据
struct Tok {
    enum Kind { REG, IMM, MEM, LABEL, IDENT } kind;
    int32_t  val = 0;       // REG:寄存器号   IMM/MEM disp:立即数
    int32_t  base = 0;      // MEM:base 寄存器号
    std::string text;       // LABEL/IDENT 名字
};
static Tok parseOperand(std::string s) {
    trim(s);
    Tok t;
    if (s.empty()) { t.kind = Tok::IDENT; return t; }
    if (s[0] == 'R' || s[0] == 'r') {
        t.kind = Tok::REG;
        t.val  = std::atoi(s.c_str() + 1);
    } else if (s[0] == '#') {
        t.kind = Tok::IMM;
        t.val  = std::atoi(s.c_str() + 1);
    } else if (s[0] == '[') {
        // [R2 + 4] 或 [R2] 或 [R2 - 4]
        t.kind = Tok::MEM;
        auto rb = s.find(']');
        std::string inner = s.substr(1, rb - 1);
        auto p = inner.find_first_of("+-");
        if (p == std::string::npos) {
            t.base = std::atoi(inner.c_str() + 1);
            t.val  = 0;
        } else {
            std::string reg = inner.substr(0, p);
            std::string dsp = inner.substr(p);
            trim(reg); trim(dsp);
            t.base = std::atoi(reg.c_str() + 1);
            t.val  = std::atoi(dsp.c_str());   // atoi 认得 "+4" / "-4"
        }
    } else {
        t.kind = Tok::IDENT;      // 可能是标签名
        t.text = s;
    }
    return t;
}

// 每种助记符对应的 Op
static const std::unordered_map<std::string, Op> kOpTab = {
    {"NOP",Op::NOP},{"MOV",Op::MOV},{"ADD",Op::ADD},{"SUB",Op::SUB},
    {"AND",Op::AND},{"OR",Op::OR},{"XOR",Op::XOR},
    {"JMP",Op::JMP},{"JZ",Op::JZ},{"CALL",Op::CALL},{"RET",Op::RET},
    {"LOAD",Op::LOAD},{"STORE",Op::STORE},{"HALT",Op::HALT},
};

int main(int argc, char** argv) {
    if (argc < 2) { std::fprintf(stderr, "usage: masm <input.asm>\n"); return 1; }
    std::ifstream in(argv[1]);
    if (!in) { std::fprintf(stderr, "open %s failed\n", argv[1]); return 2; }

    // Pass 1: 收集所有 label → pc
    std::vector<std::string> rawLines;
    std::unordered_map<std::string, uint32_t> labels;
    uint32_t pc = 0;
    std::string line;
    while (std::getline(in, line)) {
        std::string s = stripComment(line);
        trim(s);
        if (s.empty()) continue;
        // 标签?
        auto colon = s.find(':');
        if (colon != std::string::npos) {
            std::string name = s.substr(0, colon);
            trim(name);
            labels[name] = pc;
            s = s.substr(colon + 1);
            trim(s);
            if (s.empty()) continue;
        }
        rawLines.push_back(s);
        pc += 4;    // 每条指令 4 字节
    }

    // Pass 2: 生成二进制
    std::vector<uint32_t> words;
    pc = 0;
    for (auto& ln : rawLines) {
        // 提取助记符
        std::istringstream iss(ln);
        std::string mnem;
        iss >> mnem;
        for (auto& c : mnem) c = std::toupper(c);
        auto it = kOpTab.find(mnem);
        if (it == kOpTab.end()) {
            std::fprintf(stderr, "unknown mnemonic: %s\n", mnem.c_str());
            return 3;
        }
        Op op = it->second;

        // 剩余部分按逗号切成操作数
        std::string rest;
        std::getline(iss, rest);
        std::vector<Tok> args;
        {
            std::string cur;
            int bracket = 0;
            for (char c : rest) {
                if (c == '[') ++bracket;
                if (c == ']') --bracket;
                if (c == ',' && bracket == 0) { args.push_back(parseOperand(cur)); cur.clear(); }
                else cur.push_back(c);
            }
            if (!cur.empty()) args.push_back(parseOperand(cur));
        }

        // 按指令类型组装 Inst
        Inst inst = SForm{Op::NOP};
        switch (op) {
            case Op::NOP: case Op::RET: case Op::HALT:
                inst = SForm{op}; break;
            case Op::MOV: {   // MOV Rd, #imm
                inst = IForm{op, (uint8_t)args[0].val, 0, args[1].val};
            } break;
            case Op::ADD: case Op::SUB: case Op::AND: case Op::OR: case Op::XOR: {
                // 三操作数: rd, rs1, (rs2 或 #imm)
                if (args[2].kind == Tok::REG)
                    inst = RForm{op, (uint8_t)args[0].val, (uint8_t)args[1].val, (uint8_t)args[2].val};
                else
                    inst = IForm{op, (uint8_t)args[0].val, (uint8_t)args[1].val, args[2].val};
            } break;
            case Op::LOAD: case Op::STORE: {
                // OP Rd, [Rbase + disp]
                inst = MForm{op, (uint8_t)args[0].val, (uint8_t)args[1].base, args[1].val};
            } break;
            case Op::JMP: case Op::CALL: {
                uint32_t tgt = (args[0].kind == Tok::IDENT) ? labels.at(args[0].text)
                                                             : (uint32_t)args[0].val;
                inst = JForm{op, 0, tgt};
            } break;
            case Op::JZ: {
                uint32_t tgt = (args[1].kind == Tok::IDENT) ? labels.at(args[1].text)
                                                             : (uint32_t)args[1].val;
                inst = JForm{op, (uint8_t)args[0].val, tgt};
            } break;
        }
        words.push_back(encode(inst));
        pc += 4;
    }

    // 写 .bin
    std::string out = argv[1];
    auto dot = out.find_last_of('.');
    if (dot != std::string::npos) out = out.substr(0, dot);
    out += ".bin";
    std::ofstream ofs(out, std::ios::binary);
    ofs.write(reinterpret_cast<const char*>(words.data()), words.size() * 4);
    std::printf("[masm] %s -> %s  (%zu instructions, %zu bytes)\n",
        argv[1], out.c_str(), words.size(), words.size() * 4);
    return 0;
}
