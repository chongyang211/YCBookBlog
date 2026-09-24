#include "mm/pipeline.hpp"
#include "mm/predictor.hpp"
#include "mm/instruction.hpp"
#include <cassert>
#include <cstdio>
#include <vector>

using namespace mm;

// 帮手:把 Inst 数组封装成 InstMeta 数组(taken 全为 false)
static std::vector<InstMeta> wrap(const std::vector<Inst>& insts) {
    std::vector<InstMeta> r;
    uint64_t id = 0;
    uint32_t pc = 0;
    for (auto& in : insts) r.push_back({in, pc += 4, id++, false});
    return r;
}

int main() {
    // ===== 1. 无冒险纯流水: 4 条独立算术 =====
    {
        std::vector<Inst> code = {
            IForm{Op::MOV, 1, 0, 3},   // R1 = 3
            IForm{Op::MOV, 2, 0, 5},   // R2 = 5
            IForm{Op::MOV, 3, 0, 7},   // R3 = 7
            IForm{Op::MOV, 4, 0, 9},   // R4 = 9
        };
        PipelineSim sim;
        auto s = sim.simulate(wrap(code));
        std::printf("no-hazard:  N=%llu  cycles=%llu  CPI=%.2f  raw=%llu  ctrl=%llu\n",
            (unsigned long long)s.nInsts, (unsigned long long)s.nCycles,
            s.cpi(), (unsigned long long)s.nRaw, (unsigned long long)s.nControl);
        // 4 条 + 4 拍启动 = 8 拍
        assert(s.nCycles == 4 + 4);
        assert(s.nRaw == 0);
    }

    // ===== 2. 全 RAW 冒险: 后一条永远读前一条的结果 =====
    {
        std::vector<Inst> code = {
            IForm{Op::MOV, 1, 0, 3},          // R1 = 3
            IForm{Op::ADD, 2, 1, 5},          // R2 = R1 + 5      RAW on R1
            IForm{Op::ADD, 3, 2, 5},          // R3 = R2 + 5      RAW on R2
            IForm{Op::ADD, 4, 3, 5},          // R4 = R3 + 5      RAW on R3
        };
        PipelineSim sim;
        auto s = sim.simulate(wrap(code));
        std::printf("with-RAW:   N=%llu  cycles=%llu  CPI=%.2f  raw=%llu\n",
            (unsigned long long)s.nInsts, (unsigned long long)s.nCycles,
            s.cpi(), (unsigned long long)s.nRaw);
        // 4 条 + 4 拍启动 + 3 次 RAW * 1 拍 bubble = 11 拍
        assert(s.nCycles == 4 + 4 + 3);
        assert(s.nRaw == 3);
    }

    // ===== 3. 长序列 CPI 趋近 1.0 (无冒险时) =====
    {
        std::vector<Inst> code;
        for (int i = 0; i < 1000; ++i) code.push_back(IForm{Op::MOV, 1, 0, i});
        PipelineSim sim;
        auto s = sim.simulate(wrap(code));
        std::printf("long-noraw: N=%llu  cycles=%llu  CPI=%.3f\n",
            (unsigned long long)s.nInsts, (unsigned long long)s.nCycles, s.cpi());
        // 1000 条 + 4 启动 = 1004,CPI = 1.004
        assert(s.cpi() < 1.01);
    }

    // ===== 4. Predictor 独立测试 =====
    {
        Predictor p;
        // 模拟 100 次"永远跳"分支:第一次预测挂,之后逐步爬升
        for (int i = 0; i < 100; ++i) p.update(0x1000, true);
        assert(p.predict(0x1000));                     // 已进强跳
        std::printf("pred always-taken: hits=%llu misses=%llu acc=%.2f\n",
            (unsigned long long)p.hits(), (unsigned long long)p.misses(),
            p.accuracy());
        assert(p.accuracy() > 0.95);

        Predictor p2;
        // 模拟 100 次"交替跳/不跳":最坏情况, 2bit 也救不了
        for (int i = 0; i < 100; ++i) p2.update(0x2000, (i & 1) == 0);
        std::printf("pred alternate:    hits=%llu misses=%llu acc=%.2f\n",
            (unsigned long long)p2.hits(), (unsigned long long)p2.misses(),
            p2.accuracy());
        // 交替模式 2bit 命中率一般在 30-50% 之间
        assert(p2.accuracy() < 0.55);
    }

    std::puts("[OK] pipeline (data hazards)");
    return 0;
}
