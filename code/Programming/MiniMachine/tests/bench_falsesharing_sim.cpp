#include "mm/mesi.hpp"
#include "mm/memory.hpp"
#include <cstdio>
#include <chrono>

using namespace mm;

// 场景 A: 两核各写 a[0], a[1] (同一缓存行)
static void scenarioFalseShare(MesiBus& bus, CoreCache& c0, CoreCache& c1,
                                uint32_t addrA, uint32_t addrB, int N) {
    for (int i = 0; i < N; ++i) {
        c0.write8(addrA, (uint8_t)i);
        c1.write8(addrB, (uint8_t)i);
    }
}

int main() {
    constexpr int N = 100000;

    // ---- 场景 A: 伪共享 (同一缓存行内) ----
    {
        Memory mem;
        MesiBus bus;
        CoreCache c0(0, bus, mem);
        CoreCache c1(1, bus, mem);

        auto t0 = std::chrono::high_resolution_clock::now();
        scenarioFalseShare(bus, c0, c1, /*addrA=*/0x00, /*addrB=*/0x04, N);
        auto t1 = std::chrono::high_resolution_clock::now();

        double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
        std::printf("[FALSE SHARE  ] N=%d  time=%.3fms  broadcasts=%llu  invalidations=%llu\n",
            N, ms,
            (unsigned long long)bus.nBroadcasts(),
            (unsigned long long)bus.nInvalidations());
    }

    // ---- 场景 B: 不同缓存行 (相隔 64B) ----
    {
        Memory mem;
        MesiBus bus;
        CoreCache c0(0, bus, mem);
        CoreCache c1(1, bus, mem);

        auto t0 = std::chrono::high_resolution_clock::now();
        scenarioFalseShare(bus, c0, c1, /*addrA=*/0x00, /*addrB=*/0x40, N);
        auto t1 = std::chrono::high_resolution_clock::now();

        double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
        std::printf("[PADDED 64B   ] N=%d  time=%.3fms  broadcasts=%llu  invalidations=%llu\n",
            N, ms,
            (unsigned long long)bus.nBroadcasts(),
            (unsigned long long)bus.nInvalidations());
    }

    return 0;
}
