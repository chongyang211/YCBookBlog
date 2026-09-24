#include "mm/pipeline.hpp"
#include "mm/predictor.hpp"
#include "mm/instruction.hpp"
#include <algorithm>
#include <cstdio>
#include <random>
#include <vector>

using namespace mm;

// 生成"数组 arr 中 > threshold 的元素累加"这段循环的 InstMeta trace
// 关键分支:每次比较 arr[i] > threshold, 跳 = 累加, 不跳 = 跳过
static std::vector<InstMeta> genTrace(const std::vector<int>& arr, int threshold) {
    std::vector<InstMeta> trace;
    uint64_t id = 0;
    uint32_t pc = 0x1000;   // 分支指令的固定 PC(所有次分支在这里)

    for (int v : arr) {
        // 1. 加载 arr[i] (LOAD): 无分支
        trace.push_back({MForm{Op::LOAD, 1, 2, 0}, pc, id++, false});
        pc += 4;
        // 2. 比较 (SUB R3, R1, R_thr): 无分支, 结果决定 R3
        trace.push_back({RForm{Op::SUB, 3, 1, 4}, pc, id++, false});
        pc += 4;
        // 3. ★ 关键分支:JZ R3, skip  (若 R3==0 则跳过;这里简化为"if v <= threshold 就跳过")
        //    重要:PC 每次循环相同 (0x100C),预测器按这个 PC 索引
        bool taken = (v <= threshold);
        trace.push_back({JForm{Op::JZ, 3, 0x2000}, 0x100C, id++, taken});
        pc = 0x1000;   // 无论跳没跳,下一次循环回到 0x1000
    }
    return trace;
}

int main() {
    // 生成 100000 个 [0, 255] 的随机数
    std::mt19937 rng(42);
    std::vector<int> arr(100000);
    for (auto& v : arr) v = rng() & 0xFF;
    const int threshold = 128;

    // ---- 场景 A: 无序数组 ----
    auto traceUnsorted = genTrace(arr, threshold);

    // ---- 场景 B: 有序数组 (关键!) ----
    auto sortedArr = arr;
    std::sort(sortedArr.begin(), sortedArr.end());
    auto traceSorted = genTrace(sortedArr, threshold);

    auto run = [](const std::vector<InstMeta>& tr, bool usePred, const char* label) {
        PipelineSim sim;
        auto s = sim.simulate(tr, usePred);
        std::printf("%-30s  cycles=%llu  CPI=%.3f  ctrl=%llu  (%.1f%% flush)\n",
            label,
            (unsigned long long)s.nCycles, s.cpi(),
            (unsigned long long)s.nControl,
            100.0 * s.nControl / s.nInsts);
    };

    std::puts("┌────────────────────────────────────────────────────────────────┐");
    std::puts("│  ★ 分支预测打脸: 有序 vs 无序 · 有无预测器 · 四种组合          │");
    std::puts("├────────────────────────────────────────────────────────────────┤");
    run(traceUnsorted, false, "unsorted, NO predictor    ");   // 基线 1
    run(traceUnsorted, true,  "unsorted, 2bit predictor  ");   // 基线 2
    run(traceSorted,   false, "sorted,   NO predictor    ");   // 打脸 A
    run(traceSorted,   true,  "sorted,   2bit predictor  ");   // ⭐ 爽点 ⭐
    std::puts("└────────────────────────────────────────────────────────────────┘");
    return 0;
}
