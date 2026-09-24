#pragma once
#include "mm/storage.hpp"
#include <array>
#include <cstdint>
#include <list>
#include <unordered_map>
#include <vector>

namespace mm {

// 缓存参数(编译期常量, 方便优化)
struct CacheConfig {
    uint32_t sizeBytes;    // 总容量 (e.g. 32*1024)
    uint32_t assoc;         // 组相联度 (1 = 直接映射, N = N-way)
    uint32_t lineBytes;     // 行大小 (通常 64)
    uint32_t hitCycles;     // 命中延迟 (L1=1, L2=10, L3=30)
};

class Cache : public Storage {
public:
    // next_ 是下一层存储(L2/L3/Memory),miss 时递归找它
    Cache(const CacheConfig& cfg, Storage& next);

    const char* name() const override { return name_.c_str(); }
    uint8_t read8 (uint32_t addr) override;
    void    write8(uint32_t addr, uint8_t v) override;

    // 特化:允许指定名字 (e.g. "L1", "L2")
    void setName(std::string n) { name_ = std::move(n); }

    // 统计: hits/misses 已在基类 Storage 里
    void dump() const override;

private:
    // ---- 地址拆解 ----
    struct Split { uint32_t tag; uint32_t setIdx; uint32_t offset; };
    Split split(uint32_t addr) const;

    // ---- 单行数据 ----
    struct Line {
        bool     valid = false;
        bool     dirty = false;   // write-back 需要
        uint32_t tag   = 0;
        std::vector<uint8_t> data;   // 大小 = lineBytes
    };

    // 每组 N 行 + 每组一个 LRU 顺序表(存 way 编号 0..assoc-1, 头=最新)
    struct Set {
        std::vector<Line> ways;   // size = assoc
        std::list<int>    lruOrder;   // 头 = 最新访问的 way idx
    };

    // ---- 内部操作 ----
    int  findWay(Set& set, uint32_t tag);   // 命中返回 way idx, 未命中 -1
    int  chooseVictim(Set& set);            // LRU 选择被替换的 way
    void fillFromNext(Set& set, int way, uint32_t addr);
    void writeBack (const Line& line, uint32_t setIdx);   // 若 dirty 写回下一层

    CacheConfig cfg_;
    Storage&    next_;
    std::string name_ = "L?";
    std::vector<Set> sets_;
    uint32_t     nSets_ = 0;
    uint32_t     offsetBits_ = 0;
    uint32_t     indexBits_  = 0;
};

}  // namespace mm
