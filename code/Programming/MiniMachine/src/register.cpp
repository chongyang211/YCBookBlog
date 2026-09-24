#include "mm/register.hpp"
#include <cassert>

namespace mm {

uint8_t RegisterFile::read8(uint32_t addr) {
    assert(addr < kSize);
    cycles_ += kCycle;
    ++hits_;
    return data_[addr];
}

void RegisterFile::write8(uint32_t addr, uint8_t v) {
    assert(addr < kSize);
    cycles_ += kCycle;
    ++hits_;
    data_[addr] = v;
}

uint32_t RegisterFile::r(int idx) const {
    assert(idx >= 0 && idx < static_cast<int>(kCount));
    uint32_t v = 0;
    for (int i = 0; i < 4; ++i)
        v |= static_cast<uint32_t>(data_[idx * 4 + i]) << (i * 8);
    return v;
}

void RegisterFile::setR(int idx, uint32_t v) {
    assert(idx >= 0 && idx < static_cast<int>(kCount));
    for (int i = 0; i < 4; ++i)
        data_[idx * 4 + i] = static_cast<uint8_t>((v >> (i * 8)) & 0xFF);
}

}  // namespace mm
