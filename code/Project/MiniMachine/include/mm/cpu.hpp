#pragma once
#include "mm/alu.hpp"
#include "mm/instruction.hpp"
#include "mm/component.hpp"   // Component 基类
#include <array>
#include <cstdint>

namespace mm {

class Memory;  // 前向声明,避免头文件循环包含

class Cpu : public Component {
public:
    explicit Cpu(Memory& mem) : mem_(mem) {}
    const char* name() const override { return "CPU"; }
    void dump() const override;

    // 主接口
    void reset(uint32_t pc0 = 0);
    bool step();          // 执行一条指令,返回 false 表示遇到 HALT
    void run(uint64_t maxSteps = 1'000'000);  // 循环 step, maxSteps 是防死循环
    void enableTrace(bool on) { trace_ = on; }

    // 访问寄存器(测试用)
    uint32_t r(int i) const { return regs_[i]; }
    void     setR(int i, uint32_t v) { regs_[i] = v; }   // 测试/IVT 演示用
    uint32_t pc() const     { return pc_; }
    const Flags& flags() const { return alu_.flags(); }

private:
    // 三个核心步骤
    uint32_t fetch();               // 从 MEM[PC] 取 32bit
    void     execute(const Inst&);  // 按 variant 分派

    // 内存栈操作(CALL/RET 用)
    void     pushStack(uint32_t v);
    uint32_t popStack();

    std::array<uint32_t, 8> regs_{};
    uint32_t pc_      = 0;
    uint32_t sp_      = 0;      // 栈指针,阶段 ⑤ 简化:栈从内存高地址向低生长
    Alu      alu_;
    Memory&  mem_;
    bool     halt_   = false;
    bool     trace_  = false;
    uint64_t clk_    = 0;       // 时钟拍数
};

}  // namespace mm
