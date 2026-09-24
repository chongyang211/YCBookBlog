#include "mm/tlb.hpp"
#include <optional>

namespace mm {

std::optional<uint32_t> Tlb::lookup(uint32_t vpage) {
    auto it = map_.find(vpage);
    if (it == map_.end()) { ++misses_; return std::nullopt; }
    ++hits_;
    // 提到 LRU 头
    order_.erase(it->second.it);
    order_.push_front(vpage);
    it->second.it = order_.begin();
    return it->second.frame;
}

void Tlb::insert(uint32_t vpage, uint32_t frame) {
    // 满了先淘汰
    if (map_.size() >= kSize) {
        uint32_t victim = order_.back();
        order_.pop_back();
        map_.erase(victim);
    }
    order_.push_front(vpage);
    map_[vpage] = {frame, order_.begin()};
}

void Tlb::invalidate(uint32_t vpage) {
    auto it = map_.find(vpage);
    if (it == map_.end()) return;
    order_.erase(it->second.it);
    map_.erase(it);
}

void Tlb::flushAll() {
    map_.clear();
    order_.clear();
}

}  // namespace mm
