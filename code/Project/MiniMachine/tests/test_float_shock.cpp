#include <cstdio>

int main() {
    // 直接用 C++ 内置的 double 加, 已经是 IEEE754 硬件加法
    double a = 0.1, b = 0.2, c = a + b;

    // 用 %g 看起来正常
    std::printf("%g + %g = %g\n", a, b, c);
    // 用 %.20f 露馅
    std::printf("%.20f + %.20f = %.20f\n", a, b, c);

    // 用 == 判等 → 惨案现场
    std::printf("c == 0.3 ? %s\n", (c == 0.3) ? "YES" : "NO ‼");
    return 0;
}
