#pragma once
#include <cstdint>

namespace mm {

// 4 个标志位,按 x86 惯例编号
struct Flags {
    bool ZF = false;   // Zero Flag: 结果为 0
    bool SF = false;   // Sign Flag: 结果最高位为 1 (负数)
    bool CF = false;   // Carry Flag: 无符号溢出
    bool OF = false;   // Overflow Flag: 有符号溢出
    void reset() { ZF = SF = CF = OF = false; }
};

// ALU 操作码(与 Op 不同:ALU 只关心算术/逻辑)
enum class AluOp : uint8_t { ADD, SUB, AND, OR, XOR, MOV };

class Alu {
public:
    // 返回结果,同时把 flags 副产品写进 f_
    uint32_t exec(AluOp op, uint32_t a, uint32_t b);
    const Flags& flags() const { return f_; }

private:
    Flags f_;
};

}  // namespace mm
