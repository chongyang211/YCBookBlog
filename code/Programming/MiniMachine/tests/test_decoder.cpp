#include "mm/instruction.hpp"
#include <cassert>
#include <cstdio>

int main() {
    using namespace mm;

    // 1. R-form: ADD R2, R1, R3
    {
        Inst i = RForm{Op::ADD, 2, 1, 3};
        uint32_t raw = encode(i);
        Inst d = decode(raw);
        auto& r = std::get<RForm>(d);
        assert(r.op == Op::ADD && r.rd == 2 && r.rs1 == 1 && r.rs2 == 3);
        std::printf("R-form: raw=0x%08X  %s\n", raw, disasm(d).c_str());
    }
    // 2. I-form: ADD R2, R1, #5
    {
        Inst i = IForm{Op::ADD, 2, 1, 5};
        uint32_t raw = encode(i);
        Inst d = decode(raw);
        auto& r = std::get<IForm>(d);
        assert(r.op == Op::ADD && r.rd == 2 && r.rs1 == 1 && r.imm == 5);
    }
    // 3. I-form 负立即数(必须能正确符号扩展)
    {
        Inst i = IForm{Op::ADD, 2, 1, -7};
        uint32_t raw = encode(i);
        Inst d = decode(raw);
        auto& r = std::get<IForm>(d);
        assert(r.imm == -7);   // 关键:如果 sext18 写错,这里会挂
    }
    // 4. M-form: LOAD R3, [R2 + 4]
    {
        Inst i = MForm{Op::LOAD, 3, 2, 4};
        Inst d = decode(encode(i));
        auto& r = std::get<MForm>(d);
        assert(r.op == Op::LOAD && r.rd == 3 && r.base == 2 && r.disp == 4);
    }
    // 5. J-form: JMP 0x1000
    {
        Inst i = JForm{Op::JMP, 0, 0x1000};
        Inst d = decode(encode(i));
        auto& r = std::get<JForm>(d);
        assert(r.op == Op::JMP && r.target == 0x1000);
    }
    // 6. S-form: HALT
    {
        Inst d = decode(encode(SForm{Op::HALT}));
        auto& r = std::get<SForm>(d);
        assert(r.op == Op::HALT);
    }

    std::puts("[OK] decoder (encode/decode round trip)");
    return 0;
}
