// mkfs — 格式化镜像
#include "disk.hpp"
#include "layout.hpp"
#include <cstring>
#include "fmt_shim.hpp"

using namespace mlab::fs;

int main(int argc, char** argv) {
    if (argc < 2) { fmt::print("usage: mkfs <image>\n"); return 1; }
    Disk d(argv[1], true);
    uint8_t blk[BLOCK_SIZE] = {0};
    auto sb = reinterpret_cast<SuperBlock*>(blk);
    sb->magic        = 0xDEADBEEF;
    sb->block_size   = BLOCK_SIZE;
    sb->total_blocks = IMG_BLOCKS;
    sb->inode_count  = MAX_INODES;
    sb->data_first   = DATA_FIRST;
    sb->data_blocks  = IMG_BLOCKS - DATA_FIRST;
    d.write_block(0, blk);
    uint8_t zero[BLOCK_SIZE] = {0};
    d.write_block(1, zero);
    d.write_block(2, zero);
    for (int i = 0; i < 16; ++i) d.write_block(3 + i, zero);
    d.fsync();
    fmt::print("mkfs ok: {} ({} MB)\n", argv[1], IMG_BLOCKS * BLOCK_SIZE / 1024 / 1024);
    return 0;
}
