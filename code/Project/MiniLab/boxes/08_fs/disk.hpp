// disk.hpp
#pragma once
#include "layout.hpp"
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdexcept>
#include <string>

namespace mlab::fs {

class Disk {
public:
    Disk(const std::string& path, bool create = false) {
        int flags = O_RDWR | (create ? O_CREAT | O_TRUNC : 0);
        fd_ = ::open(path.c_str(), flags, 0644);
        if (fd_ < 0) throw std::runtime_error("open " + path + ": " + strerror(errno));
        if (create) {
            if (::ftruncate(fd_, IMG_BLOCKS * BLOCK_SIZE) < 0)
                throw std::runtime_error("ftruncate");
        }
    }
    ~Disk() { if (fd_ >= 0) ::close(fd_); }

    void read_block(uint32_t blk, void* buf) {
        if (::pread(fd_, buf, BLOCK_SIZE, (off_t)blk * BLOCK_SIZE) != (ssize_t)BLOCK_SIZE)
            throw std::runtime_error("pread");
    }
    void write_block(uint32_t blk, const void* buf) {
        if (::pwrite(fd_, buf, BLOCK_SIZE, (off_t)blk * BLOCK_SIZE) != (ssize_t)BLOCK_SIZE)
            throw std::runtime_error("pwrite");
    }
    void fsync() { ::fsync(fd_); }

private:
    int fd_ = -1;
};

}  // namespace mlab::fs
