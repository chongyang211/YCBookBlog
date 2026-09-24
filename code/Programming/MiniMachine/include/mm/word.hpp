#pragma once
#include <cstdint>
#include <string>

namespace mm {

// Word = 我们迷你 CPU 的字长(32bit),对应真机的一个通用寄存器
class Word {
public:
    Word() = default;
    explicit Word(uint32_t v) : v_(v) {}

    uint32_t raw() const { return v_; }
    int32_t  asSigned() const { return static_cast<int32_t>(v_); }

    // 位操作
    bool     bit(int i) const { return (v_ >> i) & 1u; }
    void     setBit(int i, bool b) {
        if (b) v_ |=  (1u << i);
        else   v_ &= ~(1u << i);
    }

    // 大小端翻转:小端 <-> 大端
    Word     endianSwap() const;

    // 打印二进制/十六进制
    std::string toBinary() const;   // "01011010 00001111 ..."
    std::string toHex()    const;   // "0x5A0F..."

private:
    uint32_t v_ = 0;
};

// ---- 纯位运算的加减法(核心:让减法变加法) ----
uint32_t addBits(uint32_t a, uint32_t b);   // 只用 ^、&、<<
uint32_t subBits(uint32_t a, uint32_t b);   // = addBits(a, ~b + 1)

// ---- 常用位运算工具 ----
int      countOnes(uint32_t v);              // popcount
uint32_t nextPow2 (uint32_t v);              // 大于等于 v 的最小 2 的幂
uint32_t reverseBits(uint32_t v);            // 位序反转

}  // namespace mm

namespace mm {

// 手写 IEEE754 单精度浮点加法(教学用,不追求性能)
// 单精度 float 布局: [sign 1bit | exp 8bit | mantissa 23bit]
float fadd(float a, float b);

// 打印 float 的位分解, 用于阶段 ③ 的教学
std::string floatDump(float f);

}  // namespace mm
