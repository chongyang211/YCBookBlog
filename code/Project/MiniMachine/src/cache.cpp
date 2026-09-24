#include "mm/cache.hpp"
#include <cassert>
#include <cstdio>
#include <cmath>
#include <algorithm>

namespace mm {

// 位数计算:log2(x),x 必须是 2 的幂
static uint32_t log2u(uint32_t x) {
    uint32_t r = 0;
    while ((1u << r) < x) ++r;
    return r;
}

Cache::Cache(const CacheConfig& cfg, Storage& next)
    : cfg_(cfg), next_(next)
{
    assert(cfg.sizeBytes % (cfg.assoc * cfg.lineBytes) == 0);
    nSets_ = cfg.sizeBytes / (cfg.assoc * cfg.lineBytes);
    offsetBits_ = log2u(cfg.lineBytes);
    indexBits_  = log2u(nSets_);
    sets_.resize(nSets_);
    for (auto& s : sets_) {
        s.ways.resize(cfg.assoc);
        for (auto& l : s.ways) l.data.resize(cfg.lineBytes, 0);
    }
}

Cache::Split Cache::split(uint32_t addr) const {
    uint32_t offset = addr & ((1u << offsetBits_) - 1);
    uint32_t setIdx = (addr >> offsetBits_) & ((1u << indexBits_) - 1);
    uint32_t tag    = addr >> (offsetBits_ + indexBits_);
    return {tag, setIdx, offset};
}

int Cache::findWay(Set& set, uint32_t tag) {
    for (size_t i = 0; i < set.ways.size(); ++i)
        if (set.ways[i].valid && set.ways[i].tag == tag)
            return static_cast<int>(i);
    return -1;
}

int Cache::chooseVictim(Set& set) {
    // 先找一个空 way,没空的话找 LRU 最尾
    for (size_t i = 0; i < set.ways.size(); ++i)
        if (!set.ways[i].valid) return static_cast<int>(i);
    return set.lruOrder.back();
}

void Cache::fillFromNext(Set& set, int way, uint32_t addr) {
    Split sp = split(addr);
    uint32_t base = addr & ~((1u << offsetBits_) - 1);   // 行首地址
    auto& line = set.ways[way];
    // 若旧行 dirty, 写回下一层
    if (line.valid && line.dirty) writeBack(line, sp.setIdx);
    // 从下一层拉一整行
    line.valid = true;
    line.dirty = false;
    line.tag   = sp.tag;
    for (uint32_t i = 0; i < cfg_.lineBytes; ++i)
        line.data[i] = next_.read8(base + i);
}

void Cache::writeBack(const Line& line, uint32_t setIdx) {
    uint32_t base = ((line.tag << indexBits_) | setIdx) << offsetBits_;
    for (uint32_t i = 0; i < cfg_.lineBytes; ++i)
        next_.write8(base + i, line.data[i]);
}

uint8_t Cache::read8(uint32_t addr) {
    Split sp = split(addr);
    Set& set = sets_[sp.setIdx];
    int way = findWay(set, sp.tag);
    if (way >= 0) {
        // 命中
        cycles_ += cfg_.hitCycles;
        ++hits_;
        // 更新 LRU: 把 way 移到头部
        set.lruOrder.remove(way);
        set.lruOrder.push_front(way);
        return set.ways[way].data[sp.offset];
    }
    // 未命中: 拉一整行
    ++misses_;
    way = chooseVictim(set);
    fillFromNext(set, way, addr);
    cycles_ += cfg_.hitCycles;   // 填充回来后的一次访问 =hit 延迟(下一层延迟已由 next_.read8 累加)
    set.lruOrder.remove(way);
    set.lruOrder.push_front(way);
    return set.ways[way].data[sp.offset];
}

void Cache::write8(uint32_t addr, uint8_t v) {
    Split sp = split(addr);
    Set& set = sets_[sp.setIdx];
    int way = findWay(set, sp.tag);
    if (way < 0) {
        // Write-allocate: 先把行拉进来
        ++misses_;
        way = chooseVictim(set);
        fillFromNext(set, way, addr);
    } else {
        ++hits_;
    }
    cycles_ += cfg_.hitCycles;
    set.ways[way].data[sp.offset] = v;
    set.ways[way].dirty = true;
    set.lruOrder.remove(way);
    set.lruOrder.push_front(way);
}

void Cache::dump() const {
    std::printf("│  %-6s : hits=%llu misses=%llu miss_rate=%.2f%% cycles=%llu\n",
        name_.c_str(),
        (unsigned long long)hits_, (unsigned long long)misses_,
        100.0 * misses_ / std::max<uint64_t>(1, hits_ + misses_),
        (unsigned long long)cycles_);
}

}  // namespace mm
