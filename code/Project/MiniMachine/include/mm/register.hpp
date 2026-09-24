#pragma once
#include "mm/storage.hpp"
#include <array>

namespace mm {

// 我们的迷你 CPU 有 8 个通用寄存器 R0..R7, 每个 4 字节, 共 32 字节
class RegisterFile : public Storage {
public:
    static constexpr uint32_t kCount = 8;
    static constexpr uint32_t kSize  = kCount * 4;   // 32 字节
    static constexpr uint32_t kCycle = 1;             // 1 拍延迟

    const char* name() const override { return "REG"; }
    uint8_t read8 (uint32_t addr) override;
    void    write8(uint32_t addr, uint8_t v) override;

    // 面向 CPU 使用的语义化接口
    uint32_t r(int idx) const;
    void     setR(int idx, uint32_t v);

private:
    std::array<uint8_t, kSize> data_{};
};

}  // namespace mm
