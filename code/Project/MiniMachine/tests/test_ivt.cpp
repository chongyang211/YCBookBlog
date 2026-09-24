#include "mm/ivt.hpp"
#include "mm/cpu.hpp"
#include "mm/memory.hpp"
#include "mm/log.hpp"
#include <cstdio>
#include <cassert>

using namespace mm;

int main() {
    Memory mem;
    Cpu cpu(mem);
    cpu.reset();
    cpu.setR(1, 100);   // R1 = 100
    cpu.setR(2, 0);     // R2 = 0

    Ivt ivt;

    // ---- 注册 #DE handler: 打印 backtrace ----
    ivt.setHandler(Ivt::VEC_DE, ExType::Fault, "#DE", [&](ExFrame& f){
        std::puts("★★★ Divide Error (#DE) triggered ★★★");
        std::printf("  PC = 0x%08X\n", f.pc);
        std::printf("  R1 = 0x%08X (%d)\n", cpu.r(1), (int32_t)cpu.r(1));
        std::printf("  R2 = 0x%08X (%d)   ← divisor is zero!\n", cpu.r(2), (int32_t)cpu.r(2));
        std::puts("  backtrace:");
        std::printf("    #0 at PC=0x%08X (%s)\n", f.pc, f.mnemonic);
        std::puts("  action: kill process (in real kernel, SIGFPE)");
    });

    // ---- 注册 syscall handler ----
    ivt.setHandler(Ivt::VEC_SYSCALL, ExType::Trap, "syscall", [&](ExFrame& f){
        uint32_t nr = cpu.r(0);   // syscall number in R0
        switch (nr) {
            case 1: {    // sys_write
                uint32_t addr = cpu.r(1);
                uint32_t len  = cpu.r(2);
                std::printf("[sys_write] ");
                for (uint32_t i = 0; i < len; ++i)
                    std::putchar(static_cast<char>(mem.read8(addr + i)));
                std::putchar('\n');
                cpu.setR(0, len);   // 返回值
            } break;
            default:
                MM_ERR("syscall", "unknown syscall nr=%u", nr);
        }
    });

    // ---- 触发 #DE ----
    ivt.raise(Ivt::VEC_DE, {Ivt::VEC_DE, /*pc=*/0x1000, /*err=*/0,
                             ExType::Fault, nullptr});

    // ---- 调用 syscall 1 (sys_write "hello") ----
    const char* msg = "hello";
    for (uint32_t i = 0; i < 5; ++i) mem.write8(0x100 + i, (uint8_t)msg[i]);
    cpu.setR(0, 1);      // syscall nr
    cpu.setR(1, 0x100);  // addr
    cpu.setR(2, 5);      // len
    ivt.raise(Ivt::VEC_SYSCALL, {Ivt::VEC_SYSCALL, /*pc=*/0x2000, /*err=*/0,
                                  ExType::Trap, nullptr});

    std::printf("IVT stats: raised=%llu handled=%llu\n",
        (unsigned long long)ivt.nRaised(),
        (unsigned long long)ivt.nHandled());
    assert(ivt.nRaised() == 2);
    return 0;
}
