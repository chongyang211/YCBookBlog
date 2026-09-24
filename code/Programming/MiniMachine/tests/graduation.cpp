// 🎓 MiniMachine 毕业答辩: 用自己写的 CPU 跑 fib(10) = 55
#include "mm/machine.hpp"
#include "mm/instruction.hpp"
#include <cstdio>

using namespace mm;

int main() {
    std::puts("╔════════════════════════════════════════════════════════════╗");
    std::puts("║   MiniMachine 毕业答辩 · Running fib.asm on your own CPU   ║");
    std::puts("╚════════════════════════════════════════════════════════════╝\n");

    Machine mach;

    // programs/fib.asm 的等价机器码 (masm 语法 → encode):
    //     MOV R1, #0        ; a = fib(0)
    //     MOV R2, #1        ; b = fib(1)
    //     MOV R3, #9        ; 迭代 9 次后 R2 = fib(10)
    // loop:
    //     ADD R4, R1, R2    ; t = a + b
    //     ADD R1, R2, #0    ; a = b   (R0 恒 0, 借 ADD 传值)
    //     ADD R2, R4, #0    ; b = t
    //     SUB R3, R3, #1    ; count--
    //     JZ   R3, done     ; count == 0 → 结束
    //     JMP  loop
    // done:
    //     HALT
    std::vector<uint32_t> fib_bin = {
        encode(IForm{Op::MOV, 1, 0, 0}),    // 0x00
        encode(IForm{Op::MOV, 2, 0, 1}),    // 0x04
        encode(IForm{Op::MOV, 3, 0, 9}),     // 0x08
        encode(RForm{Op::ADD, 4, 1, 2}),    // 0x0C  loop:
        encode(IForm{Op::ADD, 1, 2, 0}),    // 0x10
        encode(IForm{Op::ADD, 2, 4, 0}),    // 0x14
        encode(IForm{Op::SUB, 3, 3, 1}),    // 0x18
        encode(JForm{Op::JZ,  3, 36}),      // 0x1C  → done
        encode(JForm{Op::JMP, 0, 12}),      // 0x20  → loop
        encode(SForm{Op::HALT}),             // 0x24  done:
    };

    mach.load(fib_bin);
    mach.reset(/*entryPc=*/0);
    mach.run(/*maxCycles=*/1000);

    // 打印 R2 (fib(10))
    std::printf("\n>>> Final register R2 (fib(10)) = %u\n", mach.cpu().r(2));

    // 扫两遍程序区, 给 dump 大表喂点真实缓存数据 (模拟取指局部性)
    for (int pass = 0; pass < 2; ++pass)
        for (uint32_t a = 0; a < 40; ++a) (void)mach.l1().read8(a);

    // 毕业大表
    mach.dump();

    std::puts("\n🎓 恭喜!你刚才用自己写的 CPU 跑完了 fib.asm。毕业!");
    return 0;
}
