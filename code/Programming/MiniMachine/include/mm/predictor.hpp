#pragma once
#include <cstdint>
#include <array>

namespace mm {

// 2bit 饱和计数器状态: 00/01 预测不跳, 10/11 预测跳
// 每次预测正确 → 计数器往当前方向饱和(不越界)
// 每次预测错误 → 计数器往反方向走一步
class Predictor {
public:
    static constexpr int   kTableBits = 10;                     // 1024 项
    static constexpr int   kTableSize = 1 << kTableBits;
    static constexpr int   kMask      = kTableSize - 1;

    Predictor();

    bool predict(uint32_t pc) const;
    void update (uint32_t pc, bool taken);

    // 统计: 用于 benchmark
    uint64_t hits()   const { return hits_; }
    uint64_t misses() const { return misses_; }
    double   accuracy() const {
        uint64_t t = hits_ + misses_;
        return t ? double(hits_) / t : 0.0;
    }

private:
    // PC → 索引 (右移 2 位是因为指令 4 字节对齐,低 2 位永远 0)
    static uint32_t idx(uint32_t pc) { return (pc >> 2) & kMask; }

    std::array<uint8_t, kTableSize> tab_{};   // 每项 2bit,存 0..3
    mutable uint64_t hits_   = 0;
    mutable uint64_t misses_ = 0;
};

}  // namespace mm
