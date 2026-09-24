#include "mm/storage.hpp"
#include <cstdio>

namespace mm {

uint32_t Storage::read32(uint32_t addr) {
    uint32_t v = 0;
    for (int i = 0; i < 4; ++i)
        v |= static_cast<uint32_t>(read8(addr + i)) << (i * 8);
    return v;
}

void Storage::write32(uint32_t addr, uint32_t v) {
    for (int i = 0; i < 4; ++i)
        write8(addr + i, static_cast<uint8_t>((v >> (i * 8)) & 0xFF));
}

void Storage::resetStats() { cycles_ = hits_ = misses_ = 0; }

void Storage::dump() const {
    std::printf("│  %-6s : cycles=%llu hits=%llu misses=%llu\n",
        name(),
        static_cast<unsigned long long>(cycles_),
        static_cast<unsigned long long>(hits_),
        static_cast<unsigned long long>(misses_));
}

}  // namespace mm
