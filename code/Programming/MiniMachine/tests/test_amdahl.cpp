#include "mm/amdahl.hpp"
#include <cassert>
#include <cstdio>

int main() {
    // 场景 1:60% 的代码被加速 5 倍,整体加速多少?
    double x1 = mm::amdahl(0.60, 5.0);
    std::printf("amdahl(0.60, 5) = %.3f\n", x1);   // 预期 ~2.174
    assert(x1 > 2.17 && x1 < 2.18);

    // 场景 2:即便剩下 40% 完全不加速,上限也只能 2.5 倍
    double x2 = mm::amdahl(0.60, 1e9);
    std::printf("amdahl(0.60, inf) = %.3f\n", x2); // 预期 ~2.500
    assert(x2 > 2.49 && x2 < 2.51);

    std::puts("[OK] amdahl");
    return 0;
}
