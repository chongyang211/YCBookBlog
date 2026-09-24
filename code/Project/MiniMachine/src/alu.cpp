#include "mm/alu.hpp"
#include "mm/word.hpp"    // 复用阶段 ③ 的 addBits / subBits

namespace mm {

uint32_t Alu::exec(AluOp op, uint32_t a, uint32_t b) {
    uint32_t r = 0;
    f_.reset();
    switch (op) {
        case AluOp::ADD: {
            r = addBits(a, b);
            // 无符号进位:结果 < 任一加数 → 溢出
            f_.CF = (r < a);
            // 有符号溢出:同号相加得异号 → 溢出
            f_.OF = (((a ^ r) & (b ^ r)) >> 31) & 1u;
        } break;
        case AluOp::SUB: {
            r = subBits(a, b);
            f_.CF = (a < b);
            f_.OF = (((a ^ b) & (a ^ r)) >> 31) & 1u;
        } break;
        case AluOp::AND: r = a & b; break;
        case AluOp::OR:  r = a | b; break;
        case AluOp::XOR: r = a ^ b; break;
        case AluOp::MOV: r = b; break;
    }
    f_.ZF = (r == 0);
    f_.SF = (r >> 31) & 1u;
    return r;
}

}  // namespace mm
