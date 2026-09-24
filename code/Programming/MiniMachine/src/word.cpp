#include "mm/word.hpp"

namespace mm {

// -------- 补码加法(只用位运算,不用 + 号) --------
// 原理:  a XOR b  = 无进位加法
//        a AND b  = 只保留进位, 左移一位就是下一轮要加的进位
uint32_t addBits(uint32_t a, uint32_t b) {
    while (b != 0) {
        uint32_t carry = (a & b) << 1;   // 本轮进位
        a = a ^ b;                       // 本轮无进位和
        b = carry;                       // 下一轮"加数"就是进位
    }
    return a;
}

// 减法 = 加法 + 补码取反(补码:~b + 1)
uint32_t subBits(uint32_t a, uint32_t b) {
    return addBits(a, addBits(~b, 1));
}

// -------- Word 成员 --------
Word Word::endianSwap() const {
    return Word(
        ((v_ & 0x000000FFu) << 24) |
        ((v_ & 0x0000FF00u) <<  8) |
        ((v_ & 0x00FF0000u) >>  8) |
        ((v_ & 0xFF000000u) >> 24)
    );
}

std::string Word::toBinary() const {
    std::string s;
    for (int i = 31; i >= 0; --i) {
        s.push_back(((v_ >> i) & 1u) ? '1' : '0');
        if (i > 0 && i % 8 == 0) s.push_back(' ');
    }
    return s;
}

std::string Word::toHex() const {
    static const char* hx = "0123456789ABCDEF";
    std::string s = "0x";
    for (int i = 28; i >= 0; i -= 4)
        s.push_back(hx[(v_ >> i) & 0xF]);
    return s;
}

// -------- 位运算工具 --------
int countOnes(uint32_t v) {
    int c = 0;
    while (v) { c += v & 1u; v >>= 1; }
    return c;
}

uint32_t nextPow2(uint32_t v) {
    if (v <= 1) return 1;
    --v;
    v |= v >> 1;  v |= v >> 2;  v |= v >> 4;
    v |= v >> 8;  v |= v >> 16;
    return v + 1;
}

uint32_t reverseBits(uint32_t v) {
    v = ((v >> 1) & 0x55555555u) | ((v & 0x55555555u) << 1);
    v = ((v >> 2) & 0x33333333u) | ((v & 0x33333333u) << 2);
    v = ((v >> 4) & 0x0F0F0F0Fu) | ((v & 0x0F0F0F0Fu) << 4);
    v = ((v >> 8) & 0x00FF00FFu) | ((v & 0x00FF00FFu) << 8);
    return (v >> 16) | (v << 16);
}

}  // namespace mm

#include <cstring>
#include <cmath>

namespace mm {

namespace {

// 拆解 float → 三个字段
// ⚠️ 不用位域: bitfield 的分配顺序(LSB-first / MSB-first)是实现定义的,
//    clang/arm64 上 sign:1, exp:8, mant:23 会把 sign 放到 bit0, 与 IEEE754 布局相反。
//    手动位运算才是可移植写法(也是本案例阶段 ③ 排错表第 2 条的修法)。
static uint32_t fSign(uint32_t u) { return u >> 31; }
static uint32_t fExp (uint32_t u) { return (u >> 23) & 0xFF; }
static uint32_t fMant(uint32_t u) { return u & 0x7FFFFF; }

uint32_t unpack(float f) {
    uint32_t u; std::memcpy(&u, &f, 4); return u;
}
float pack(uint32_t sign, uint32_t exp, uint32_t mant) {
    uint32_t u = (sign << 31) | (exp << 23) | (mant & 0x7FFFFF);
    float f; std::memcpy(&f, &u, 4); return f;
}

}  // namespace

float fadd(float a, float b) {
    uint32_t ua = unpack(a), ub = unpack(b);
    uint32_t xa = fSign(ua), xb = fSign(ub);
    uint32_t xe = fExp(ua),  ye = fExp(ub);
    uint32_t xm = fMant(ua), ym = fMant(ub);

    // 1. 特殊值直接兜底(简化处理:遇到 inf/NaN/0 就回落到硬件加法)
    if (xe == 0xFF || ye == 0xFF || (xe == 0 && xm == 0)
                                      || (ye == 0 && ym == 0)) {
        return a + b;
    }

    // 2. 补上"隐藏的 1", 再左移 3 位留出保护位 (guard/round/sticky)
    uint32_t mx = (xm | (1u << 23)) << 3;
    uint32_t my = (ym | (1u << 23)) << 3;

    // 3. 对齐指数:小的指数往上调, 尾数右移(sticky 位保留"曾经截断"的信息)
    auto rshiftSticky = [](uint32_t v, int s) -> uint32_t {
        if (s == 0) return v;
        if (s >= 27) return v != 0 ? 1u : 0u;
        return (v >> s) | ((v & ((1u << s) - 1)) != 0 ? 1u : 0u);
    };
    int ex = static_cast<int>(xe);
    int ey = static_cast<int>(ye);
    if (ex > ey) { my = rshiftSticky(my, ex - ey); ey = ex; }
    else         { mx = rshiftSticky(mx, ey - ex); ex = ey; }

    // 4. 处理符号: 同号相加, 异号相减
    int64_t sum;
    if (xa == xb) sum = static_cast<int64_t>(mx) + static_cast<int64_t>(my);
    else          sum = static_cast<int64_t>(mx) - static_cast<int64_t>(my);

    uint32_t rs = xa;
    if (sum < 0) { rs = 1 - rs; sum = -sum; }
    if (sum == 0) return 0.0f;

    // 5. 归一化到 [1<<26, 1<<27) (24bit 尾数 + 3bit 保护位)
    while (sum >= (1LL << 27)) { sum = (sum >> 1) | (sum & 1); ++ex; }
    while (sum > 0 && sum < (1LL << 26)) { sum <<= 1; --ex; }

    // 6. 舍入: round-to-nearest-even (IEEE754 硬件默认舍入模式)
    uint32_t m24   = static_cast<uint32_t>(sum >> 3);
    uint32_t guard = static_cast<uint32_t>(sum & 7);
    if (guard > 4 || (guard == 4 && (m24 & 1))) ++m24;
    if (m24 >= (1u << 24)) { m24 >>= 1; ++ex; }   // 舍入进位恰好越过 2.0

    // 7. 打包回去(尾数抹掉隐藏的 1)
    if (ex >= 0xFF) return xa ? -HUGE_VALF : HUGE_VALF;  // 溢出
    if (ex <= 0)    return 0.0f;                         // 下溢简化处理
    return pack(rs, static_cast<uint32_t>(ex), m24 & 0x7FFFFFu);
}

std::string floatDump(float f) {
    uint32_t u = unpack(f);
    char buf[128];
    std::snprintf(buf, sizeof(buf), "sign=%u exp=%u(bias 127 → %d) mant=0x%06X",
        (unsigned)fSign(u), (unsigned)fExp(u),
        static_cast<int>(fExp(u)) - 127, (unsigned)fMant(u));
    return buf;
}

}  // namespace mm
