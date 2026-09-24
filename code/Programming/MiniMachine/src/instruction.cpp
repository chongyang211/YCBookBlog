#include "mm/instruction.hpp"
#include <cassert>
#include <cstdio>
#include <string>

namespace mm {

// 位字段常量
constexpr int OP_SHIFT   = 27, OP_MASK   = 0x1F;   // 5 bit
constexpr int RD_SHIFT   = 24, RD_MASK   = 0x07;   // 3 bit
constexpr int RS1_SHIFT  = 21, RS1_MASK  = 0x07;
constexpr int MODE_SHIFT = 18, MODE_MASK = 0x07;
constexpr int OPERAND_MASK = 0x3FFFF;              // 18 bit

// 18bit 有符号数扩展到 32bit
static int32_t sext18(uint32_t v) {
    v &= OPERAND_MASK;
    if (v & (1u << 17)) v |= ~OPERAND_MASK;   // 符号位在第 17 位
    return static_cast<int32_t>(v);
}

// -------- 编码 --------
uint32_t encode(const Inst& inst) {
    uint32_t r = 0;
    std::visit([&](auto&& x) {
        using T = std::decay_t<decltype(x)>;
        r |= (static_cast<uint32_t>(x.op) & OP_MASK) << OP_SHIFT;
        if constexpr (std::is_same_v<T, RForm>) {
            r |= (x.rd  & RD_MASK)  << RD_SHIFT;
            r |= (x.rs1 & RS1_MASK) << RS1_SHIFT;
            r |= (static_cast<uint32_t>(Mode::REG) & MODE_MASK) << MODE_SHIFT;
            r |= (x.rs2 & 0x07);
        } else if constexpr (std::is_same_v<T, IForm>) {
            r |= (x.rd  & RD_MASK)  << RD_SHIFT;
            r |= (x.rs1 & RS1_MASK) << RS1_SHIFT;
            r |= (static_cast<uint32_t>(Mode::IMM) & MODE_MASK) << MODE_SHIFT;
            r |= (static_cast<uint32_t>(x.imm) & OPERAND_MASK);
        } else if constexpr (std::is_same_v<T, MForm>) {
            r |= (x.rd   & RD_MASK)  << RD_SHIFT;
            r |= (x.base & RS1_MASK) << RS1_SHIFT;
            r |= (static_cast<uint32_t>(Mode::BAS) & MODE_MASK) << MODE_SHIFT;
            r |= (static_cast<uint32_t>(x.disp) & OPERAND_MASK);
        } else if constexpr (std::is_same_v<T, JForm>) {
            r |= (x.rs1 & RS1_MASK) << RS1_SHIFT;
            r |= (static_cast<uint32_t>(Mode::DIR) & MODE_MASK) << MODE_SHIFT;
            r |= (x.target & OPERAND_MASK);
        } else /* SForm */ {
            // 只写 op,其它字段 0
        }
    }, inst);
    return r;
}

// -------- 解码 --------
Inst decode(uint32_t raw) {
    Op   op   = static_cast<Op>  ((raw >> OP_SHIFT)   & OP_MASK);
    uint8_t rd   = (raw >> RD_SHIFT)   & RD_MASK;
    uint8_t rs1  = (raw >> RS1_SHIFT)  & RS1_MASK;
    Mode mode = static_cast<Mode>((raw >> MODE_SHIFT) & MODE_MASK);
    uint32_t operand = raw & OPERAND_MASK;

    switch (op) {
        case Op::NOP: case Op::RET: case Op::HALT:
            return SForm{op};
        case Op::JMP: case Op::CALL: case Op::JZ:
            return JForm{op, rs1, operand};
        case Op::LOAD: case Op::STORE:
            return MForm{op, rd, rs1, sext18(operand)};
        default:  // MOV/ADD/SUB/AND/OR/XOR
            if (mode == Mode::REG) {
                return RForm{op, rd, rs1, static_cast<uint8_t>(operand & 0x07)};
            } else {
                return IForm{op, rd, rs1, sext18(operand)};
            }
    }
}

// -------- 反汇编 --------
std::string opName(Op op) {
    static const char* tab[] = {
        "NOP","MOV","ADD","SUB","AND","OR","XOR",
        "JMP","JZ","CALL","RET","LOAD","STORE","HALT"
    };
    int i = static_cast<int>(op);
    return (i >= 0 && i < 14) ? tab[i] : "???";
}
std::string modeName(Mode m) {
    static const char* tab[] = {"IMM","REG","DIR","IND","BAS","?","?","?"};
    return tab[static_cast<int>(m) & 7];
}

std::string disasm(const Inst& inst) {
    char buf[128] = {0};
    std::visit([&](auto&& x) {
        using T = std::decay_t<decltype(x)>;
        if constexpr (std::is_same_v<T, RForm>) {
            std::snprintf(buf, sizeof(buf), "%-5s R%u, R%u, R%u",
                opName(x.op).c_str(), x.rd, x.rs1, x.rs2);
        } else if constexpr (std::is_same_v<T, IForm>) {
            if (x.op == Op::MOV)
                std::snprintf(buf, sizeof(buf), "MOV   R%u, #%d", x.rd, x.imm);
            else
                std::snprintf(buf, sizeof(buf), "%-5s R%u, R%u, #%d",
                    opName(x.op).c_str(), x.rd, x.rs1, x.imm);
        } else if constexpr (std::is_same_v<T, MForm>) {
            const char* fmt = (x.op == Op::STORE) ? "STORE R%u, [R%u + %d]"
                                                  : "LOAD  R%u, [R%u + %d]";
            std::snprintf(buf, sizeof(buf), fmt, x.rd, x.base, x.disp);
        } else if constexpr (std::is_same_v<T, JForm>) {
            if (x.op == Op::JZ)
                std::snprintf(buf, sizeof(buf), "JZ    R%u, 0x%X", x.rs1, x.target);
            else
                std::snprintf(buf, sizeof(buf), "%-5s 0x%X", opName(x.op).c_str(), x.target);
        } else /* SForm */ {
            std::snprintf(buf, sizeof(buf), "%s", opName(x.op).c_str());
        }
    }, inst);
    return buf;
}

}  // namespace mm
