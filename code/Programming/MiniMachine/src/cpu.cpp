#include "mm/cpu.hpp"
#include "mm/memory.hpp"
#include "mm/log.hpp"
#include <cstdio>
#include <variant>

namespace mm {

void Cpu::reset(uint32_t pc0) {
    regs_.fill(0);
    pc_    = pc0;
    sp_    = Memory::kSize - 4;    // 栈顶从内存最高处开始
    halt_  = false;
    clk_   = 0;
    alu_   = {};
}

uint32_t Cpu::fetch() {
    uint32_t raw = mem_.read32(pc_);
    return raw;
}

// 内存栈:向低地址生长
void Cpu::pushStack(uint32_t v) { sp_ -= 4; mem_.write32(sp_, v); }
uint32_t Cpu::popStack()        { uint32_t v = mem_.read32(sp_); sp_ += 4; return v; }

void Cpu::execute(const Inst& inst) {
    uint32_t next_pc = pc_ + 4;    // 默认顺序执行

    std::visit([&](auto&& x) {
        using T = std::decay_t<decltype(x)>;

        // ---- 算术/逻辑,三寄存器 ----
        if constexpr (std::is_same_v<T, RForm>) {
            AluOp aop;
            switch (x.op) {
                case Op::ADD: aop = AluOp::ADD; break;
                case Op::SUB: aop = AluOp::SUB; break;
                case Op::AND: aop = AluOp::AND; break;
                case Op::OR:  aop = AluOp::OR;  break;
                case Op::XOR: aop = AluOp::XOR; break;
                default: return;
            }
            regs_[x.rd] = alu_.exec(aop, regs_[x.rs1], regs_[x.rs2]);
        }
        // ---- 算术/MOV, 带立即数 ----
        else if constexpr (std::is_same_v<T, IForm>) {
            AluOp aop;
            switch (x.op) {
                case Op::MOV: aop = AluOp::MOV; break;
                case Op::ADD: aop = AluOp::ADD; break;
                case Op::SUB: aop = AluOp::SUB; break;
                case Op::AND: aop = AluOp::AND; break;
                case Op::OR:  aop = AluOp::OR;  break;
                case Op::XOR: aop = AluOp::XOR; break;
                default: return;
            }
            uint32_t b = static_cast<uint32_t>(x.imm);
            regs_[x.rd] = alu_.exec(aop, regs_[x.rs1], b);
        }
        // ---- LOAD/STORE ----
        else if constexpr (std::is_same_v<T, MForm>) {
            uint32_t addr = regs_[x.base] + static_cast<uint32_t>(x.disp);
            if (x.op == Op::LOAD)       regs_[x.rd] = mem_.read32(addr);
            else /* STORE */            mem_.write32(addr, regs_[x.rd]);
        }
        // ---- 跳转 ----
        else if constexpr (std::is_same_v<T, JForm>) {
            switch (x.op) {
                case Op::JMP:  next_pc = x.target; break;
                case Op::JZ:   if (regs_[x.rs1] == 0) next_pc = x.target; break;
                case Op::CALL: pushStack(pc_ + 4);   next_pc = x.target; break;
                default: break;
            }
        }
        // ---- NOP/RET/HALT ----
        else /* SForm */ {
            switch (x.op) {
                case Op::HALT: halt_ = true; break;
                case Op::RET:  next_pc = popStack(); break;
                default: break;   // NOP
            }
        }
    }, inst);

    pc_ = next_pc;
}

bool Cpu::step() {
    if (halt_) return false;
    uint32_t raw = fetch();
    Inst inst = decode(raw);
    if (trace_) {
        std::printf("[CLK %6llu] PC=%08X  %08X   %s\n",
            (unsigned long long)clk_, pc_, raw, disasm(inst).c_str());
    }
    execute(inst);
    ++clk_;
    return !halt_;
}

void Cpu::run(uint64_t maxSteps) {
    for (uint64_t i = 0; i < maxSteps && !halt_; ++i) {
        if (!step()) break;
    }
    if (!halt_) MM_WARN("cpu", "hit maxSteps=%llu, likely infinite loop",
        (unsigned long long)maxSteps);
}

void Cpu::dump() const {
    std::printf("│  CPU    : PC=%08X  clk=%llu  halt=%d\n",
        pc_, (unsigned long long)clk_, (int)halt_);
    for (int i = 0; i < 8; ++i) {
        std::printf("│    R%d = 0x%08X (%d)\n", i, regs_[i], (int32_t)regs_[i]);
    }
    std::printf("│    FLAGS: ZF=%d SF=%d CF=%d OF=%d\n",
        alu_.flags().ZF, alu_.flags().SF, alu_.flags().CF, alu_.flags().OF);
}

}  // namespace mm
