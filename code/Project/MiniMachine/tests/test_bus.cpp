#include "mm/bus.hpp"
#include "mm/memory.hpp"
#include <cstdio>
#include <cassert>

using namespace mm;

int main() {
    // 4 个 master (id=0..3),1 个 slave (memory)
    Memory mem;

    auto runScenario = [&](std::unique_ptr<IArbiter> arb, const char* label) {
        SystemBus bus(std::move(arb));
        bus.addSlave({0, Memory::kSize, &mem});

        // 4 个 master 每人 100 个请求
        for (int m = 0; m < 4; ++m) {
            for (int i = 0; i < 100; ++i) {
                bus.request({m, static_cast<uint32_t>((i * 4) % Memory::kSize),
                             false, 0, 0});
            }
        }
        int served[4] = {};
        while (true) {
            auto info = bus.tick();
            if (info.masterId < 0) break;
            ++served[info.masterId];
        }
        std::printf("[%s] serve dist: m0=%d m1=%d m2=%d m3=%d  avgLat=%.2f maxLat=%llu\n",
            label, served[0], served[1], served[2], served[3],
            bus.avgLatency(), (unsigned long long)bus.maxLatency());
    };

    runScenario(std::make_unique<RoundRobinArbiter>(), "RR ");
    runScenario(std::make_unique<PriorityArbiter>(),    "PRI");

    std::puts("[OK] bus (arbiters)");
    return 0;
}
