// bitmap.hpp
#pragma once
#include <cstdint>
#include <cstring>
#include <vector>

namespace mlab::fs {

class Bitmap {
public:
    explicit Bitmap(std::size_t n) : data_((n + 7) / 8, 0), n_(n) {}
    bool get(std::size_t i)   const { return (data_[i / 8] >> (i & 7)) & 1; }
    void set(std::size_t i)         { data_[i / 8] |=  (1 << (i & 7)); }
    void clear(std::size_t i)       { data_[i / 8] &= ~(1 << (i & 7)); }

    long find_first_zero() const {
        for (std::size_t i = 0; i < n_; ++i) if (!get(i)) return (long)i;
        return -1;
    }
    int popcount() const {
        int c = 0; for (std::size_t i = 0; i < n_; ++i) if (get(i)) ++c; return c;
    }
    void load_from(const uint8_t* s) { std::memcpy(data_.data(), s, data_.size()); }
    void save_to  (uint8_t* d) const { std::memcpy(d, data_.data(), data_.size()); }

private:
    std::vector<uint8_t> data_;
    std::size_t n_;
};

}  // namespace mlab::fs
