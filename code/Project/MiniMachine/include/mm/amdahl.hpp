#pragma once
namespace mm {

// s = 加速部分在原程序中占比 (0~1)
// p = 加速部分自身被加速多少倍
// 返回:整体加速比
constexpr double amdahl(double s, double p) {
    return 1.0 / ((1.0 - s) + s / p);
}

}  // namespace mm
