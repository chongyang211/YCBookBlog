// buddy.hpp — Buddy 系统 + 异或求伙伴
#pragma once
#include "arena.hpp"
#include <algorithm>
#include <array>
#include <bitset>
#include <stdexcept>
#include <vector>

namespace mlab::mm {

class Buddy {
public:
    static constexpr std::size_t NUM_PAGES = ARENA_SIZE / PAGE_BYTES;

    explicit Buddy(Arena& a) : arena_(a) {
        block_order_.fill(-1);
        free_list_[MAX_ORDER].push_back(0);
        block_order_[0] = MAX_ORDER;
        is_free_.set(0);
    }

    long alloc(int order) {
        if (order < 0 || order > MAX_ORDER) return -1;
        int cur = order;
        while (cur <= MAX_ORDER && free_list_[cur].empty()) ++cur;
        if (cur > MAX_ORDER) return -1;
        long page = free_list_[cur].back(); free_list_[cur].pop_back();
        is_free_.reset(page);
        while (cur > order) {
            --cur;
            long buddy = page + (1L << cur);
            free_list_[cur].push_back(buddy);
            block_order_[buddy] = cur;
            is_free_.set(buddy);
        }
        block_order_[page] = order;
        return page * PAGE_BYTES;
    }

    void free(long offset) {
        long page = offset / PAGE_BYTES;
        int order = block_order_[page];
        if (order < 0) throw std::runtime_error("double free or bad ptr");
        block_order_[page] = -1;
        while (order < MAX_ORDER) {
            long buddy = page ^ (1L << order);
            if (buddy >= (long)NUM_PAGES) break;
            if (!is_free_.test(buddy)) break;
            if (block_order_[buddy] != order) break;
            auto& fl = free_list_[order];
            fl.erase(std::remove(fl.begin(), fl.end(), buddy), fl.end());
            is_free_.reset(buddy);
            block_order_[buddy] = -1;
            page = std::min(page, buddy);
            ++order;
            block_order_[page] = order;
        }
        free_list_[order].push_back(page);
        is_free_.set(page);
    }

    static int order_for(std::size_t bytes) {
        std::size_t pages = (bytes + PAGE_BYTES - 1) / PAGE_BYTES;
        int order = 0;
        while ((1u << order) < pages) ++order;
        return order;
    }

    std::array<int, MAX_ORDER + 1> free_counts() const {
        std::array<int, MAX_ORDER + 1> r{};
        for (int o = 0; o <= MAX_ORDER; ++o) r[o] = (int)free_list_[o].size();
        return r;
    }
    std::size_t total_free_bytes() const {
        std::size_t s = 0;
        for (int o = 0; o <= MAX_ORDER; ++o)
            s += free_list_[o].size() * (1ULL << o) * PAGE_BYTES;
        return s;
    }

private:
    Arena& arena_;
    std::array<std::vector<long>, MAX_ORDER + 1> free_list_;
    std::array<int, NUM_PAGES> block_order_;
    std::bitset<NUM_PAGES>     is_free_;
};

}  // namespace mlab::mm
