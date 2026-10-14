// arena.hpp — mmap 预分配
#pragma once
#include <sys/mman.h>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string>
#include <errno.h>

// macOS 用 MAP_ANON，Linux 用 MAP_ANONYMOUS
#ifndef MAP_ANONYMOUS
#  ifdef MAP_ANON
#    define MAP_ANONYMOUS MAP_ANON
#  endif
#endif

namespace mlab::mm {

constexpr std::size_t ARENA_SIZE = 16 * 1024 * 1024;
constexpr std::size_t PAGE_BYTES = 4 * 1024;
constexpr int         MAX_ORDER  = 12;

class Arena {
public:
    Arena() {
        base_ = static_cast<uint8_t*>(mmap(nullptr, ARENA_SIZE,
            PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
        if (base_ == MAP_FAILED)
            throw std::runtime_error(std::string("mmap: ") + strerror(errno));
    }
    ~Arena() { if (base_) munmap(base_, ARENA_SIZE); }
    uint8_t* at(std::size_t offset) { return base_ + offset; }
    std::size_t offset_of(void* p) const { return static_cast<uint8_t*>(p) - base_; }
    bool contains(void* p) const {
        auto u = static_cast<uint8_t*>(p);
        return u >= base_ && u < base_ + ARENA_SIZE;
    }
    uint8_t* base() { return base_; }
private:
    uint8_t* base_ = nullptr;
};

}  // namespace mlab::mm
