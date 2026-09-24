#include "mm/vm.hpp"
#include "mm/memory.hpp"
#include <cstdio>
#include <cassert>

using namespace mm;

int main() {
    Memory phys;
    // 物理帧数 = 16 (足够教学)
    VirtualMemory vm(phys, 16);

    // ---- 场景 1: 先手动映射 V=0x1000 → 任意物理帧, 正常读写 ----
    {
        // 手动 map (模拟"内核态启动时建映射")
        vm.mmu().mapPage(0x1000, /*frame=*/5, /*writable=*/true);
        vm.write8(0x1000, 42);
        uint8_t v = vm.read8(0x1000);
        assert(v == 42);
        std::puts("[OK] mapped page: read/write works");
    }

    // ---- 场景 2: ★ 写从未映射的地址 → 触发缺页 → handler 分配帧 ----
    {
        std::puts("\n★★★ 造 BUG 现场: 写虚拟地址 0x5000 (未映射) ★★★");
        vm.write8(0x5000, 0x77);   // 触发 PageFault, 由 handler 分配 + 重试
        assert(vm.read8(0x5000) == 0x77);
        std::puts("[OK] page fault handled: allocated + retry successful");
    }

    // ---- 场景 3: 大量顺序访问 → TLB 命中率 > 95% ----
    {
        for (int i = 0; i < 100; ++i) vm.write8(0x10000 + i, (uint8_t)i);
        for (int i = 0; i < 100; ++i) assert(vm.read8(0x10000 + i) == (uint8_t)i);
        std::printf("TLB hits=%llu misses=%llu hitRate=%.2f%%\n",
            (unsigned long long)vm.tlb().hits(),
            (unsigned long long)vm.tlb().misses(),
            100.0 * vm.tlb().hitRate());
        assert(vm.tlb().hitRate() > 0.95);
    }

    // ---- 场景 4: 打印完整页表(相当于 gdb `info pagetable`) ----
    vm.dumpPageTable();

    std::puts("\n[OK] vm (page fault handling + TLB)");
    return 0;
}
