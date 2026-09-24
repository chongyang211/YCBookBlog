#pragma once
#include "mm/component.hpp"
#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>

namespace mm {

// 所有存储部件的统一接口:寄存器/内存/缓存/磁盘 都实现它
class Storage : public Component {
public:
    // 每一层都实现自己的 read/write, 并累加 cycles_
    virtual uint8_t  read8 (uint32_t addr) = 0;
    virtual void     write8(uint32_t addr, uint8_t v) = 0;

    // 便捷方法:读/写 32bit(小端), 默认基于 read8/write8 拼装
    uint32_t read32 (uint32_t addr);
    void     write32(uint32_t addr, uint32_t v);

    // 统计
    uint64_t cycles() const { return cycles_; }
    uint64_t hits()   const { return hits_; }
    uint64_t misses() const { return misses_; }
    void     resetStats();

    void dump() const override;

protected:
    uint64_t cycles_ = 0;
    uint64_t hits_   = 0;
    uint64_t misses_ = 0;
};

}  // namespace mm
