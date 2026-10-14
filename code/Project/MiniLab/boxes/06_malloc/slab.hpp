// slab.hpp — 按 size class 的 Slab 分配器
#pragma once
#include "arena.hpp"
#include "buddy.hpp"
#include <array>
#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace mlab::mm {

constexpr std::array<int, 8> SIZE_CLASS = {16, 32, 64, 128, 256, 512, 1024, 2048};

struct SlabPage {
    long page_offset;
    int  obj_size;
    int  cap;
    int  in_use = 0;
    long freelist_head = -1;
};

class Slab {
public:
    Slab(Arena& a, Buddy& b) : arena_(a), buddy_(b) {
        for (std::size_t i = 0; i < SIZE_CLASS.size(); ++i)
            class_idx_[SIZE_CLASS[i]] = i;
    }

    long alloc(int bytes) {
        int cls = pick_class(bytes);
        if (cls < 0) return -1;
        auto& pages = class_pages_[cls];
        for (auto* p : pages) if (p->freelist_head != -1) return pop(p);
        long pg = buddy_.alloc(0);
        if (pg < 0) return -1;
        auto* sp = new SlabPage;
        sp->page_offset = pg;
        sp->obj_size    = SIZE_CLASS[cls];
        sp->cap         = PAGE_BYTES / sp->obj_size;
        init_freelist(*sp);
        pages.push_back(sp);
        page_to_slab_[pg] = sp;
        return pop(sp);
    }

    void free(long off) {
        long pg = (off / PAGE_BYTES) * PAGE_BYTES;
        auto it = page_to_slab_.find(pg);
        if (it == page_to_slab_.end())
            throw std::runtime_error("Slab::free: unknown ptr");
        auto* sp = it->second;
        *reinterpret_cast<long*>(arena_.at(off)) = sp->freelist_head;
        sp->freelist_head = off;
        sp->in_use--;
    }

    int reap() {
        int freed = 0;
        for (auto& [cls, pages] : class_pages_) {
            auto it = pages.begin();
            while (it != pages.end()) {
                auto* sp = *it;
                if (sp->in_use == 0) {
                    buddy_.free(sp->page_offset);
                    page_to_slab_.erase(sp->page_offset);
                    delete sp;
                    it = pages.erase(it);
                    ++freed;
                } else ++it;
            }
        }
        return freed;
    }

    struct ClassStat { int obj_size; int pages; int in_use; int cached; };
    std::vector<ClassStat> stats() const {
        std::vector<ClassStat> r;
        for (auto& [cls, pages] : class_pages_) {
            int in_use = 0, cached = 0;
            for (auto* p : pages) { in_use += p->in_use; cached += (p->cap - p->in_use); }
            r.push_back({SIZE_CLASS[cls], (int)pages.size(), in_use, cached});
        }
        return r;
    }

private:
    int pick_class(int b) const {
        for (std::size_t i = 0; i < SIZE_CLASS.size(); ++i)
            if (b <= SIZE_CLASS[i]) return (int)i;
        return -1;
    }
    void init_freelist(SlabPage& sp) {
        long head = -1;
        for (int i = sp.cap - 1; i >= 0; --i) {
            long obj = sp.page_offset + (long)i * sp.obj_size;
            *reinterpret_cast<long*>(arena_.at(obj)) = head;
            head = obj;
        }
        sp.freelist_head = head;
    }
    long pop(SlabPage* sp) {
        long obj = sp->freelist_head;
        sp->freelist_head = *reinterpret_cast<long*>(arena_.at(obj));
        sp->in_use++;
        return obj;
    }

    Arena& arena_;
    Buddy& buddy_;
    std::unordered_map<int, int> class_idx_;
    std::unordered_map<int, std::vector<SlabPage*>> class_pages_;
    std::unordered_map<long, SlabPage*> page_to_slab_;
};

}  // namespace mlab::mm
