#include "mm/memory.hpp"
#include "mm/log.hpp"
#include <cassert>
#include <cstring>

namespace mm {

Memory::Memory() : bytes_(kSize, 0) {}

uint8_t Memory::read8(uint32_t addr) {
    assert(addr < kSize);
    cycles_ += kCycle;
    ++misses_;   // 对内存而言,不涉及缓存, 全记为 miss (语义:未从更快的层拿到)
    return bytes_[addr];
}

void Memory::write8(uint32_t addr, uint8_t v) {
    assert(addr < kSize);
    cycles_ += kCycle;
    ++misses_;
    bytes_[addr] = v;
}

void Memory::loadBinary(const std::vector<uint8_t>& b, uint32_t at) {
    assert(at + b.size() <= kSize);
    std::memcpy(bytes_.data() + at, b.data(), b.size());
    MM_INFO("mem", "loaded %zu bytes at 0x%08X", b.size(), at);
}

}  // namespace mm
