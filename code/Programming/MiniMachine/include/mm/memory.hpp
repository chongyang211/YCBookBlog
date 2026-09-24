#pragma once
#include "mm/storage.hpp"
#include <vector>

namespace mm {

class Memory : public Storage {
public:
    // 阶段 ② 只做 64KB, 阶段 ⑨ 扩到 16MB + 页表
    static constexpr uint32_t kSize  = 64 * 1024;
    static constexpr uint32_t kCycle = 100;      // 100 拍延迟

    Memory();
    const char* name() const override { return "MEM"; }
    uint8_t read8 (uint32_t addr) override;
    void    write8(uint32_t addr, uint8_t v) override;

    // 阶段 ⑤ 加载 .bin 用
    void loadBinary(const std::vector<uint8_t>& bytes, uint32_t at = 0);

private:
    std::vector<uint8_t> bytes_;
};

}  // namespace mm
