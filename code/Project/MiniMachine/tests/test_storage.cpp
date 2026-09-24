#include "mm/memory.hpp"
#include "mm/register.hpp"
#include <cstdio>
#include <cassert>

int main() {
    mm::Memory mem;

    // 先给 64KB 内存塞点数据
    for (uint32_t i = 0; i < mem.kSize; ++i)
        mem.write8(i, static_cast<uint8_t>(i & 0xFF));

    // 场景 A:连续读 100 字节 (顺序 addr = 0..99)
    mem.resetStats();
    for (uint32_t i = 0; i < 100; ++i) (void)mem.read8(i);
    uint64_t cyc_seq = mem.cycles();

    // 场景 B:跳读 100 字节 (每次隔 4KB, 模拟"随机访问")
    mem.resetStats();
    for (uint32_t i = 0; i < 100; ++i) (void)mem.read8(i * 4096 % mem.kSize);
    uint64_t cyc_rnd = mem.cycles();

    std::printf("cyc_seq = %llu, cyc_rnd = %llu  (ratio = %.1f)\n",
        (unsigned long long)cyc_seq,
        (unsigned long long)cyc_rnd,
        double(cyc_rnd) / double(cyc_seq));

    // 阶段 ② 阶段, 两者都是 100 * 100 = 10000 拍, 比值 = 1.0
    // 【这就是为什么要加缓存!】等阶段 ⑦ 加了 Cache, ratio 会飙到 ~8x
    assert(cyc_seq == cyc_rnd);

    // 顺便测 RegisterFile 语义
    mm::RegisterFile rf;
    rf.setR(3, 0xDEADBEEF);
    assert(rf.r(3) == 0xDEADBEEF);
    std::printf("R3 = 0x%08X  (reg cycles=%llu)\n",
        rf.r(3), (unsigned long long)rf.cycles());

    std::puts("[OK] storage");
    return 0;
}
