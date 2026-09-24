#include "mm/mesi.hpp"
#include "mm/memory.hpp"
#include <cassert>
#include <cstdio>

using namespace mm;

int main() {
    Memory mem;
    MesiBus bus;
    CoreCache c0(0, bus, mem);
    CoreCache c1(1, bus, mem);

    // 场景 1:两核先后读同一地址,应该都进 S 态
    (void)c0.read8(0x100);   // C0: I → E
    (void)c1.read8(0x100);   // C1: BusRd → C0 说"我有" → C0: E→S, C1: I→S
    assert(c0.transitions[static_cast<int>(MState::E)][static_cast<int>(MState::S)] == 1);
    assert(c1.transitions[static_cast<int>(MState::I)][static_cast<int>(MState::S)] == 1);
    std::puts("[OK] shared read: both enter S state");

    // 场景 2:C0 写共享行,C1 的副本作废
    c0.write8(0x100, 0xAA);   // C0: S→M (BusUpgr) → C1: S→I
    assert(c0.transitions[static_cast<int>(MState::S)][static_cast<int>(MState::M)] == 1);
    assert(c1.transitions[static_cast<int>(MState::S)][static_cast<int>(MState::I)] == 1);
    assert(bus.nInvalidations() == 1);
    std::puts("[OK] write invalidates other core's copy");

    // 场景 3:C1 现在读,发现 C0 是 M 态,C0 要写回,两边 S
    (void)c1.read8(0x100);   // C1: I → BusRd → C0: M→S(写回), C1: I→S
    assert(c0.transitions[static_cast<int>(MState::M)][static_cast<int>(MState::S)] == 1);
    std::puts("[OK] BusRd forces M holder to writeback and downgrade to S");

    // dump 一下,肉眼看完整状态迁移
    std::puts("┌─ MESI 状态迁移统计 ─────────────────────────┐");
    c0.dump(); c1.dump();
    std::printf("│  BUS: broadcasts=%llu invalidations=%llu\n",
        (unsigned long long)bus.nBroadcasts(),
        (unsigned long long)bus.nInvalidations());
    std::puts("└────────────────────────────────────────────┘");

    std::puts("[OK] mesi (all 3 scenarios)");
    return 0;
}
