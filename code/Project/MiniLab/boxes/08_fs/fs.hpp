// fs.hpp — 迷你 inode FS
#pragma once
#include "bitmap.hpp"
#include "disk.hpp"
#include "layout.hpp"
#include <algorithm>
#include <cstring>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace mlab::fs {

struct Dentry { std::string name; uint32_t inum; };

class FS {
public:
    explicit FS(const std::string& path)
      : disk_(path, false),
        ino_bmap_(MAX_INODES),
        data_bmap_(IMG_BLOCKS) {
        load_meta();
    }

    void load_meta() {
        uint8_t blk[BLOCK_SIZE];
        disk_.read_block(0, blk);
        auto sb = reinterpret_cast<SuperBlock*>(blk);
        if (sb->magic != 0xDEADBEEF)
            throw std::runtime_error("bad magic — run mkfs first");
        disk_.read_block(1, blk); ino_bmap_.load_from(blk);
        disk_.read_block(2, blk); data_bmap_.load_from(blk);
    }

    void save_meta() {
        uint8_t blk[BLOCK_SIZE] = {0};
        ino_bmap_.save_to(blk);  disk_.write_block(1, blk);
        std::memset(blk, 0, BLOCK_SIZE);
        data_bmap_.save_to(blk); disk_.write_block(2, blk);
    }

    DiskInode read_inode(uint32_t inum) {
        if (inum >= MAX_INODES) throw std::runtime_error("inum oob");
        uint32_t blk = 3 + (inum * INODE_SIZE) / BLOCK_SIZE;
        uint32_t off = (inum * INODE_SIZE) % BLOCK_SIZE;
        uint8_t b[BLOCK_SIZE]; disk_.read_block(blk, b);
        DiskInode ino; std::memcpy(&ino, b + off, INODE_SIZE);
        return ino;
    }
    void write_inode(uint32_t inum, const DiskInode& ino) {
        uint32_t blk = 3 + (inum * INODE_SIZE) / BLOCK_SIZE;
        uint32_t off = (inum * INODE_SIZE) % BLOCK_SIZE;
        uint8_t b[BLOCK_SIZE]; disk_.read_block(blk, b);
        std::memcpy(b + off, &ino, INODE_SIZE);
        disk_.write_block(blk, b);
    }

    uint32_t alloc_inode() {
        long i = ino_bmap_.find_first_zero();
        if (i < 0) throw std::runtime_error("no free inode");
        ino_bmap_.set(i);
        DiskInode ino{}; ino.type = 1; write_inode(i, ino);
        return i;
    }
    uint32_t alloc_data_block() {
        long i = data_bmap_.find_first_zero();
        if (i < 0) throw std::runtime_error("no free block");
        data_bmap_.set(i);
        return DATA_FIRST + i;
    }
    void free_data_block(uint32_t blk) { data_bmap_.clear(blk - DATA_FIRST); }

    uint32_t create(const std::string& name) {
        for (auto& d : root_) if (d.name == name) throw std::runtime_error("exists");
        uint32_t inum = alloc_inode();
        root_.push_back({name, inum});
        auto ino = read_inode(inum);
        ino.nlink = 1; write_inode(inum, ino);
        save_meta();
        return inum;
    }

    void write_all(uint32_t inum, const std::vector<uint8_t>& data) {
        if (data.size() > MAX_FILE_BYTES) throw std::runtime_error("too big");
        auto ino = read_inode(inum);
        free_blocks_of(ino);
        ino = read_inode(inum);
        std::memset(ino.direct, 0, sizeof(ino.direct));
        ino.indirect = 0;
        std::size_t nb = (data.size() + BLOCK_SIZE - 1) / BLOCK_SIZE;
        for (std::size_t i = 0; i < nb; ++i) {
            uint32_t blk = alloc_data_block();
            uint8_t buf[BLOCK_SIZE] = {0};
            std::size_t copy = std::min(BLOCK_SIZE, data.size() - i * BLOCK_SIZE);
            std::memcpy(buf, data.data() + i * BLOCK_SIZE, copy);
            disk_.write_block(blk, buf);
            set_block_at(ino, i, blk);
        }
        ino.size = data.size();
        write_inode(inum, ino);
        save_meta();
    }

    std::vector<uint8_t> read_all(uint32_t inum) {
        auto ino = read_inode(inum);
        std::vector<uint8_t> out(ino.size);
        std::size_t nb = (ino.size + BLOCK_SIZE - 1) / BLOCK_SIZE;
        for (std::size_t i = 0; i < nb; ++i) {
            uint8_t buf[BLOCK_SIZE];
            disk_.read_block(get_block_at(ino, i), buf);
            std::size_t copy = std::min(BLOCK_SIZE,
                (std::size_t)ino.size - i * BLOCK_SIZE);
            std::memcpy(out.data() + i * BLOCK_SIZE, buf, copy);
        }
        return out;
    }

    void unlink(const std::string& name) {
        for (auto it = root_.begin(); it != root_.end(); ++it) {
            if (it->name == name) {
                uint32_t inum = it->inum;
                root_.erase(it);
                auto ino = read_inode(inum);
                ino.nlink--;
                write_inode(inum, ino);
                try_release(inum);
                save_meta();
                return;
            }
        }
        throw std::runtime_error("not found");
    }

    int open(const std::string& name) {
        for (auto& d : root_) if (d.name == name) {
            open_count_[d.inum]++;
            int fd = next_fd_++;
            fd_table_[fd] = d.inum;
            return fd;
        }
        throw std::runtime_error("not found");
    }
    void close(int fd) {
        auto it = fd_table_.find(fd);
        if (it == fd_table_.end()) throw std::runtime_error("bad fd");
        uint32_t inum = it->second;
        fd_table_.erase(it);
        if (--open_count_[inum] == 0) try_release(inum);
    }

    struct Stat { int files; int inodes_used; int data_blocks_used; int zombies; };
    Stat stat() const {
        Stat s{};
        s.files = root_.size();
        s.inodes_used = ino_bmap_.popcount();
        s.data_blocks_used = data_bmap_.popcount();
        for (auto& [in, c] : open_count_)
            if (c > 0 && !has_dentry(in)) s.zombies++;
        return s;
    }
    const std::vector<Dentry>& list_root() const { return root_; }
    bool is_opened(uint32_t inum) const {
        auto it = open_count_.find(inum);
        return it != open_count_.end() && it->second > 0;
    }

private:
    bool has_dentry(uint32_t inum) const {
        for (auto& d : root_) if (d.inum == inum) return true;
        return false;
    }
    void try_release(uint32_t inum) {
        auto ino = read_inode(inum);
        if (ino.nlink > 0) return;
        if (is_opened(inum)) return;      // ⭐ BUG-5 的关键
        free_blocks_of(ino);
        std::memset(&ino, 0, sizeof(ino));
        write_inode(inum, ino);
        ino_bmap_.clear(inum);
    }
    void free_blocks_of(const DiskInode& ino) {
        std::size_t nb = (ino.size + BLOCK_SIZE - 1) / BLOCK_SIZE;
        for (std::size_t i = 0; i < nb; ++i) {
            DiskInode cp = ino;
            free_data_block(get_block_at(cp, i));
        }
        if (ino.indirect) free_data_block(ino.indirect);
    }
    uint32_t get_block_at(DiskInode& ino, std::size_t idx) {
        if (idx < DIRECT_PTRS) return ino.direct[idx];
        uint32_t table[INDIRECT_PTRS];
        disk_.read_block(ino.indirect, table);
        return table[idx - DIRECT_PTRS];
    }
    void set_block_at(DiskInode& ino, std::size_t idx, uint32_t blk) {
        if (idx < DIRECT_PTRS) { ino.direct[idx] = blk; return; }
        if (ino.indirect == 0) ino.indirect = alloc_data_block();
        uint32_t table[INDIRECT_PTRS];
        disk_.read_block(ino.indirect, table);
        table[idx - DIRECT_PTRS] = blk;
        disk_.write_block(ino.indirect, table);
    }

    Disk disk_;
    Bitmap ino_bmap_;
    Bitmap data_bmap_;
    std::vector<Dentry> root_;
    std::unordered_map<uint32_t, int> open_count_;
    std::unordered_map<int, uint32_t> fd_table_;
    int next_fd_ = 3;
};

}  // namespace mlab::fs
