#include "mm/word.hpp"
#include <cassert>
#include <cstdio>
#include <cmath>
#include <cstdint>

int main() {
    using namespace mm;

    // === Part 1: 位运算加减法 ===
    assert(addBits(3, 5) == 8);
    assert(addBits(100, 200) == 300);
    assert(addBits(0xFFFFFFFFu, 1) == 0);   // 溢出环绕
    assert(subBits(10, 3) == 7);
    assert(subBits(3, 10) == static_cast<uint32_t>(-7));

    // === Part 2: Word / 大小端 ===
    Word w(0x11223344);
    assert(w.endianSwap().raw() == 0x44332211);

    // === Part 3: 位工具 ===
    assert(countOnes(0xFFu) == 8);
    assert(nextPow2(65) == 128);
    assert(reverseBits(0x80000000u) == 0x00000001u);
    std::puts("[OK] word (part 1~3)");

    // === Part 4: IEEE754 浮点加法(教学重头戏) ===
    float f = fadd(0.1f, 0.2f);
    std::printf("fadd(0.1, 0.2) = %.17f\n", f);
    assert(f == (0.1f + 0.2f));

    // 用 == 判浮点相等的经典打脸(需 double 精度才必然失败)
    double d = 0.1 + 0.2;
    std::printf("(double) 0.1+0.2 = %.20f\n", d);
    assert(d != 0.3);   // ← 这里成立!说明打脸真实存在

    // 正确姿势:用 epsilon
    assert(std::fabs(d - 0.3) < 1e-9);

    std::puts("[OK] word (float part 4)");
    return 0;
}
