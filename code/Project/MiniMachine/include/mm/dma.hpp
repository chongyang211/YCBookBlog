#pragma once
#include "mm/io.hpp"
#include "mm/memory.hpp"
#include <deque>

namespace mm {

class DmaEngine {
public:
    static constexpr uint32_t kBytesPerTick = 8;   // 每拍搬 8 字节
    static constexpr uint32_t kDoneIrq       = 42;

    DmaEngine(Memory& mem, IoController& io) : mem_(mem), io_(io) {}

    // 提交任务: 从 src 拷贝 n 字节到 dst,完成时触发 irq
    struct Task {
        uint32_t src, dst, remaining;
        uint32_t irq;
    };
    void submit(uint32_t src, uint32_t dst, uint32_t n, uint32_t irq = kDoneIrq) {
        queue_.push_back({src, dst, n, irq});
    }

    // 每拍推进
    // 返回本拍是否忙 (供 CPU 侧统计"CPU 占用率")
    bool tick();

    bool busy() const { return !queue_.empty(); }
    uint64_t nCompleted() const { return nDone_; }

private:
    Memory&        mem_;
    IoController&  io_;
    std::deque<Task> queue_;
    uint64_t nDone_ = 0;
};

}  // namespace mm
