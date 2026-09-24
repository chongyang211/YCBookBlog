#include "mm/cache.hpp"
#include "mm/memory.hpp"
#include <cassert>
#include <cstdio>

using namespace mm;

int main() {
    // ===== 1. 直接映射 (1-way),很小的缓存故意造冲突 =====
    {
        Memory mem;
        // 4 行 * 64B = 256B 缓存, 直接映射
        Cache l1({.sizeBytes = 256, .assoc = 1, .lineBytes = 64, .hitCycles = 1}, mem);
        l1.setName("L1(1w)");

        // 写 4 个地址,每个刚好在不同 set
        for (int i = 0; i < 4; ++i) l1.write8(i * 64, (uint8_t)i);
        for (int i = 0; i < 4; ++i) assert(l1.read8(i * 64) == i);

        // 现在写 0x100 (= 4*64), 与 addr=0 同一 set → 直接映射会踢掉 addr=0
        l1.write8(0x100, 42);
        assert(l1.read8(0x100) == 42);      // 命中
        assert(l1.read8(0)     == 0);        // 已被踢, 从 mem 重新填, 但值仍然是 0
        std::printf("  [1-way]  hits=%llu misses=%llu (预期约 5:4)\n",
            (unsigned long long)l1.hits(), (unsigned long long)l1.misses());
    }

    // ===== 2. 4-way 组相联,同样场景不再冲突 =====
    {
        Memory mem;
        // 1 组 * 4 ways * 64B = 256B (退化为全相联)
        Cache l1({.sizeBytes = 256, .assoc = 4, .lineBytes = 64, .hitCycles = 1}, mem);
        l1.setName("L1(4w)");

        for (int i = 0; i < 4; ++i) l1.write8(i * 64, (uint8_t)i);
        l1.write8(0x100, 42);
        // 4-way 里, 0x100 挤走 LRU (addr=0),但访问顺序里 addr=0 是最老 → 挤走 addr=0
        assert(l1.read8(0x100) == 42);
        // addr=0 现在还能命中吗? 视 LRU 顺序而定
        std::printf("  [4-way]  hits=%llu misses=%llu\n",
            (unsigned long long)l1.hits(), (unsigned long long)l1.misses());
        assert(l1.misses() < 6);  // 应比直接映射少
    }

    // ===== 3. 三级缓存串联: L1(32KB, 8-way) → L2(256KB) → L3(2MB) → Mem =====
    {
        Memory mem;
        Cache l3({.sizeBytes = 2*1024*1024, .assoc = 16, .lineBytes = 64, .hitCycles = 30}, mem);
        l3.setName("L3");
        Cache l2({.sizeBytes = 256*1024,    .assoc = 8,  .lineBytes = 64, .hitCycles = 10}, l3);
        l2.setName("L2");
        Cache l1({.sizeBytes = 32*1024,     .assoc = 8,  .lineBytes = 64, .hitCycles = 1 }, l2);
        l1.setName("L1");

        // 顺序访问 16KB 一遍, 再第二遍 → 第二遍应该几乎全 L1 命中
        for (int pass = 0; pass < 2; ++pass) {
            for (int i = 0; i < 16*1024; i += 4) (void)l1.read8(i);
        }
        std::printf("--- three-level cache scan ---\n");
        l1.dump(); l2.dump(); l3.dump();
        std::printf("mem cycles=%llu\n", (unsigned long long)mem.cycles());

        // 断言: L1 命中率 ≥ 90% (第二遍全命中, 加上第一遍每 16 次一 miss)
        assert(l1.hits() * 100 / (l1.hits() + l1.misses()) >= 90);
    }
    std::puts("[OK] three-level cache");

    std::puts("[OK] cache (mapping + LRU)");
    return 0;
}
