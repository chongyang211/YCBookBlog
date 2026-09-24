#pragma once
#include <cstdint>
#include <list>
#include <unordered_map>

namespace mm {

class Tlb {
public:
    static constexpr int kSize = 64;

    // 查:找到则返回 frame, 未命中返回 nullopt
    std::optional<uint32_t> lookup(uint32_t vpage);
    // 插:虚页 → 物理帧
    void insert(uint32_t vpage, uint32_t frame);
    // 失效:mapPage 后要 flush
    void invalidate(uint32_t vpage);
    void flushAll();

    uint64_t hits()   const { return hits_; }
    uint64_t misses() const { return misses_; }
    double hitRate() const {
        uint64_t t = hits_ + misses_;
        return t ? double(hits_) / t : 0.0;
    }

private:
    // vpage → (frame, list_iterator)
    struct Entry { uint32_t frame; std::list<uint32_t>::iterator it; };
    std::unordered_map<uint32_t, Entry> map_;
    std::list<uint32_t> order_;   // 头 = 最新
    uint64_t hits_ = 0, misses_ = 0;
};

}  // namespace mm
