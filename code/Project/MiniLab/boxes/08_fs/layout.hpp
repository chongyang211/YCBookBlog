// layout.hpp — 磁盘上结构
#pragma once
#include <cstdint>
#include <cstddef>

namespace mlab::fs {

constexpr std::size_t BLOCK_SIZE    = 4096;
constexpr std::size_t INODE_SIZE    = 256;
constexpr std::size_t MAX_INODES    = 256;
constexpr std::size_t IMG_BLOCKS    = 4096;
constexpr std::size_t DATA_FIRST    = 1 + 1 + 1 + 16;  // super + 2 bmap + 16 inode blks
constexpr std::size_t DIRECT_PTRS   = 12;
constexpr std::size_t INDIRECT_PTRS = BLOCK_SIZE / sizeof(uint32_t);
constexpr std::size_t MAX_FILE_BYTES = DIRECT_PTRS * BLOCK_SIZE + 512 * BLOCK_SIZE;

struct DiskInode {
    uint32_t type;
    uint32_t size;
    uint32_t nlink;
    uint32_t reserved;
    uint32_t direct[DIRECT_PTRS];
    uint32_t indirect;
    uint32_t padding[INODE_SIZE / sizeof(uint32_t) - 4 - DIRECT_PTRS - 1];
};
static_assert(sizeof(DiskInode) == INODE_SIZE, "inode size != 256");

struct SuperBlock {
    uint32_t magic;
    uint32_t block_size;
    uint32_t total_blocks;
    uint32_t inode_count;
    uint32_t data_first;
    uint32_t data_blocks;
};

}  // namespace mlab::fs
