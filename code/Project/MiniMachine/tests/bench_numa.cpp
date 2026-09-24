#include "mm/numa.hpp"
#include <cstdio>
#include <chrono>

using namespace mm;

int main() {
    // ---- 场景 A: 就近访问 ---- (core 0 访问 node 0 的地址)
    {
        NumaSystem sys;
        for (int i = 0; i < 10000; ++i)
            sys.write8(/*core=*/0, /*addr=*/(i * 4) % NumaSystem::kMemPerNode,
                       (uint8_t)i);
        std::printf("[LOCAL  ] local=%llu remote=%llu totalCyc=%llu\n",
            (unsigned long long)sys.localAccess(),
            (unsigned long long)sys.remoteAccess(),
            (unsigned long long)sys.totalCycles());
    }
    // ---- 场景 B: ★ 跨节点错配 ---- (core 0 访问 node 1 的地址)
    {
        NumaSystem sys;
        for (int i = 0; i < 10000; ++i)
            sys.write8(/*core=*/0, /*addr=*/NumaSystem::kMemPerNode + (i * 4) % NumaSystem::kMemPerNode,
                       (uint8_t)i);
        std::printf("[REMOTE ] local=%llu remote=%llu totalCyc=%llu\n",
            (unsigned long long)sys.localAccess(),
            (unsigned long long)sys.remoteAccess(),
            (unsigned long long)sys.totalCycles());
    }
    // ---- 场景 C: 就近访问 + 打印 speedup ----
    // (略, 直接对比 A 和 B 的 totalCyc)
    return 0;
}
