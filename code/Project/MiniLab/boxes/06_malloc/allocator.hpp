// allocator.hpp — 顶层统一入口
#pragma once
#include "arena.hpp"
#include "buddy.hpp"
#include "slab.hpp"
#include <stdexcept>
#include <unordered_map>

namespace mlab::mm {

class Allocator {
public:
    Allocator() : buddy_(arena_), slab_(arena_, buddy_) {}

    void* alloc(std::size_t bytes) {
        if (bytes == 0) return nullptr;
        long off = -1;
        if (bytes <= 2048) {
            off = slab_.alloc((int)bytes);
            if (off >= 0) big_[off] = false;
        }
        if (off < 0) {
            int order = Buddy::order_for(bytes);
            off = buddy_.alloc(order);
            if (off < 0) return nullptr;
            big_[off] = true;
        }
        sizes_[off] = bytes;
        return arena_.at(off);
    }

    void free(void* p) {
        if (!p) return;
        if (!arena_.contains(p)) throw std::runtime_error("free: outside arena");
        long off = arena_.offset_of(p);
        auto it = big_.find(off);
        if (it == big_.end()) throw std::runtime_error("free: unknown ptr");
        if (it->second) buddy_.free(off);
        else            slab_.free(off);
        big_.erase(it); sizes_.erase(off);
    }

    int reap() { return slab_.reap(); }

    std::size_t buddy_free_bytes()   const { return buddy_.total_free_bytes(); }
    auto buddy_counts()               const { return buddy_.free_counts(); }
    auto slab_stats()                 const { return slab_.stats(); }
    std::size_t user_in_use() const {
        std::size_t s = 0; for (auto& [_, sz] : sizes_) s += sz; return s;
    }

private:
    Arena arena_;
    Buddy buddy_;
    Slab  slab_;
    std::unordered_map<long, bool>         big_;
    std::unordered_map<long, std::size_t>  sizes_;
};

}  // namespace mlab::mm
