#include "mm/dma.hpp"
#include "mm/memory.hpp"
#include "mm/io.hpp"
#include <cstdio>
#include <atomic>

using namespace mm;

int main() {
    constexpr uint32_t kBytes = 32 * 1024;   // 32KB 拷贝(源 + 目的刚好占满 64KB 内存)

    // ---- 场景 A: CPU 手工拷贝 ----
    {
        Memory mem;
        // 预填源数据
        for (uint32_t i = 0; i < kBytes; ++i) mem.write8(i, (uint8_t)i);
        mem.resetStats();

        // CPU 每拍搬 1 字节(极简);统计"CPU 忙拍数"
        uint64_t cpuBusy = 0;
        for (uint32_t i = 0; i < kBytes; ++i) {
            uint8_t v = mem.read8(i);
            mem.write8(kBytes + i, v);
            cpuBusy += 2;   // 一读一写
        }
        std::printf("[CPU COPY  ] bytes=%u cpuBusyTicks=%llu memCycles=%llu\n",
            kBytes, (unsigned long long)cpuBusy, (unsigned long long)mem.cycles());
    }

    // ---- 场景 B: DMA 拷贝 ----
    {
        Memory mem;
        for (uint32_t i = 0; i < kBytes; ++i) mem.write8(i, (uint8_t)i);
        mem.resetStats();

        IoController io;
        DmaEngine dma(mem, io);

        bool done = false;
        io.registerIrq(DmaEngine::kDoneIrq, [&done](uint32_t irq){
            std::printf("  [IRQ %u] DMA done, CPU received interrupt\n", irq);
            done = true;
        });

        // CPU 只需要 1 拍下发命令
        uint64_t cpuBusy = 1;
        dma.submit(0, kBytes, kBytes);

        // 模拟并发:每拍 CPU 干别的活(空转),DMA 自己搬
        uint64_t totalTicks = 0;
        while (!done) {
            dma.tick();
            ++totalTicks;
            // CPU 这拍如果 dma 忙,就没占用总线,可以干别的 → 不算 cpuBusy
        }
        std::printf("[DMA COPY  ] bytes=%u cpuBusyTicks=%llu totalTicks=%llu memCycles=%llu\n",
            kBytes, (unsigned long long)cpuBusy,
            (unsigned long long)totalTicks,
            (unsigned long long)mem.cycles());
        std::printf("  CPU 占用比:   %.2f%% (vs 100%%)\n",
            100.0 * cpuBusy / totalTicks);
    }
    return 0;
}
