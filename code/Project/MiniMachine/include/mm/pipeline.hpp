#pragma once
#include "mm/instruction.hpp"
#include <array>
#include <cstdint>
#include <optional>
#include <vector>

namespace mm {

// 每条指令在流水线里的元数据
struct InstMeta {
    Inst      inst;      // 原始指令
    uint32_t  pc;        // 该指令的 PC
    uint64_t  id;        // 该指令是第几条(单增)
    bool      taken;     // 若是分支,实际跳没跳(执行时确定)
};

// 冒险类型
enum class Hazard { None, RAW, Control };

// 每种冒险的 bubble 拍数(教学取值)
constexpr int kRawBubble       = 1;   // 加了前递, 大多数 RAW 只损失 1 拍
constexpr int kControlBubble   = 2;   // 预测错时冲刷 IF + ID 两级

class PipelineSim {
public:
    // 一次性喂入一条指令流(from 阶段 ⑤ Cpu trace),返回统计
    struct Stats {
        uint64_t nInsts    = 0;
        uint64_t nCycles   = 0;
        uint64_t nRaw      = 0;
        uint64_t nControl  = 0;
        double   cpi()      const { return nInsts ? double(nCycles) / nInsts : 0.0; }
    };

    Stats simulate(const std::vector<InstMeta>& trace, bool useBranchPred = true);

private:
    // 读寄存器号(源1/源2)和写寄存器号(rd),不同 Form 不同
    static int  writeReg(const Inst&);       // rd, 无写 → -1
    static void readRegs(const Inst&, int r[2]);   // 填 rs1/rs2, 无源 → -1
    static bool isBranch(const Inst&);
    static bool isJump  (const Inst&);
    static bool hasRaw  (const Inst& prev, const Inst& cur);
};

}  // namespace mm
