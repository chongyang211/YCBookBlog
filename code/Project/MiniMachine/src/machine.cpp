#include "mm/machine.hpp"
#include "mm/log.hpp"
#include <cstdio>
#include <fstream>
#include <iterator>
#include <vector>

namespace mm {

Machine::Machine()
    : memory_(),
      l3_({.sizeBytes = 2u * 1024 * 1024, .assoc = 16, .lineBytes = 64, .hitCycles = 30}, memory_),
      l2_({.sizeBytes = 256u * 1024,     .assoc = 8,  .lineBytes = 64, .hitCycles = 10}, l3_),
      l1_({.sizeBytes = 32u * 1024,      .assoc = 8,  .lineBytes = 64, .hitCycles = 1 }, l2_),
      vm_(memory_, 16),   // 64KB 物理内存 = 16 个 4KB 帧
      cpu_(memory_),
      bus_(std::make_unique<RoundRobinArbiter>()),
      io_(),
      dma_(memory_, io_),
      ivt_()
{
    l1_.setName("L1");
    l2_.setName("L2");
    l3_.setName("L3");
    bus_.addSlave({0, Memory::kSize, &memory_});
    wireSyscallHandlers();
    wireExceptionHandlers();
    MM_INFO("machine",
        "MiniMachine assembled: Memory %uB, L1/L2/L3 caches, MMU+TLB+VM, CPU, Bus+DMA, IVT ready",
        Memory::kSize);
}

void Machine::wireSyscallHandlers() {
    ivt_.setHandler(Ivt::VEC_SYSCALL, ExType::Trap, "syscall", [this](ExFrame&) {
        uint32_t nr = cpu_.r(0);
        switch (nr) {
            case 1: {   // sys_write(fd=R1, buf=R2, len=R3) 简化: fd 忽略
                uint32_t addr = cpu_.r(2), len = cpu_.r(3);
                std::printf("[sys_write] ");
                for (uint32_t i = 0; i < len; ++i)
                    std::putchar((char)memory_.read8(addr + i));
                std::putchar('\n');
                MM_INFO("syscall", "sys_write done (%u bytes)", len);
            } break;
            case 60: {  // sys_exit
                MM_INFO("machine", "sys_exit(%u)", cpu_.r(1));
                halted_ = true;
            } break;
            default:
                MM_ERR("syscall", "unknown nr=%u", nr);
        }
    });
}

void Machine::wireExceptionHandlers() {
    ivt_.setHandler(Ivt::VEC_DE, ExType::Fault, "#DE", [this](ExFrame& f) {
        MM_ERR("cpu", "★ Divide Error at PC=0x%X → halt", f.pc);
        halted_ = true;
    });
    ivt_.setHandler(Ivt::VEC_PF, ExType::Fault, "#PF", [](ExFrame& f) {
        // 教学说明: 真正的按需分配在 VirtualMemory::onPageFault 内部完成,
        // 向量表这一层只做登记演示
        MM_WARN("mmu", "★ Page Fault at 0x%X (vmm handles)", f.errCode);
    });
}

void Machine::load(const std::vector<uint32_t>& program, uint32_t base) {
    for (size_t i = 0; i < program.size(); ++i)
        memory_.write32(base + i * 4, program[i]);
    MM_INFO("machine", "loaded %zu instructions at base=0x%X", program.size(), base);
}

void Machine::cmdLoad(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) { MM_ERR("mach", "load failed: %s", path.c_str()); return; }
    std::vector<uint8_t> bytes((std::istreambuf_iterator<char>(in)), {});
    memory_.loadBinary(bytes, 0);
    reset(0);
    MM_INFO("mach", "loaded %zu bytes, PC=0x0", bytes.size());
}

void Machine::reset(uint32_t entryPc) {
    cpu_.reset(entryPc);
    halted_    = false;
    ranCycles_ = 0;
}

void Machine::run(uint64_t maxCycles) {
    while (!halted_ && ranCycles_ < maxCycles) {
        if (!cpu_.step()) { halted_ = true; break; }   // HALT
        ++ranCycles_;
    }
    if (ranCycles_ >= maxCycles && !halted_)
        MM_WARN("machine", "hit maxCycles=%llu — possibly infinite loop",
            (unsigned long long)maxCycles);
}

// ── 毕业答辩:全链路 dump ──
static double hitRatePct(uint64_t h, uint64_t m) {
    return h + m ? 100.0 * double(h) / double(h + m) : 0.0;
}

void Machine::dump() {
    std::puts("");
    std::puts("╔════════════════════════════════════════════════════════════════╗");
    std::puts("║          🎓 MiniMachine 全链路 状态大表 (毕业答辩)                ║");
    std::puts("╠════════════════════════════════════════════════════════════════╣");
    std::printf("║ [阶段 ⑤ CPU]  ranCycles=%llu halted=%d PC=0x%08X\n",
        (unsigned long long)ranCycles_, (int)halted_, cpu_.pc());
    std::printf("║              regs: R0=%u R1=%u R2=%u R3=%u\n",
        cpu_.r(0), cpu_.r(1), cpu_.r(2), cpu_.r(3));
    std::printf("║ [阶段 ② MEM]  cycles=%llu\n",
        (unsigned long long)memory_.cycles());
    std::printf("║ [阶段 ⑥ L1 ]  hits=%llu misses=%llu hitRate=%.1f%%\n",
        (unsigned long long)l1_.hits(), (unsigned long long)l1_.misses(),
        hitRatePct(l1_.hits(), l1_.misses()));
    std::printf("║ [阶段 ⑥ L2 ]  hits=%llu misses=%llu hitRate=%.1f%%\n",
        (unsigned long long)l2_.hits(), (unsigned long long)l2_.misses(),
        hitRatePct(l2_.hits(), l2_.misses()));
    std::printf("║ [阶段 ⑥ L3 ]  hits=%llu misses=%llu hitRate=%.1f%%\n",
        (unsigned long long)l3_.hits(), (unsigned long long)l3_.misses(),
        hitRatePct(l3_.hits(), l3_.misses()));
    std::printf("║ [阶段 ⑨ TLB]  hits=%llu misses=%llu hitRate=%.1f%%\n",
        (unsigned long long)vm_.tlb().hits(), (unsigned long long)vm_.tlb().misses(),
        vm_.tlb().hitRate() * 100);
    std::printf("║ [阶段 ⑨ VMM]  pageFaults=%llu freeFrames=%u\n",
        (unsigned long long)vm_.nFaults(), vm_.framesFree());
    std::printf("║ [阶段 ⑩ BUS]  served=%llu avgLat=%.2f maxLat=%llu\n",
        (unsigned long long)bus_.nServed(), bus_.avgLatency(),
        (unsigned long long)bus_.maxLatency());
    std::printf("║ [阶段 ⑪ IO ]  irqsRaised=%llu dmaTasksDone=%llu\n",
        (unsigned long long)io_.nIrqs(), (unsigned long long)dma_.nCompleted());
    std::printf("║ [阶段 ⑫ IVT]  raised=%llu handled=%llu\n",
        (unsigned long long)ivt_.nRaised(), (unsigned long long)ivt_.nHandled());
    std::puts("╚════════════════════════════════════════════════════════════════╝");
}

}  // namespace mm
