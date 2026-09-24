#pragma once
#include "mm/mmu.hpp"
#include "mm/tlb.hpp"
#include "mm/memory.hpp"

namespace mm {

// 一个物理帧分配器(free list)
class FrameAllocator {
public:
    explicit FrameAllocator(uint32_t nFrames);
    uint32_t alloc();   // 抛异常若没帧了
    void     free(uint32_t frame);
    uint32_t freeCount() const { return static_cast<uint32_t>(free_.size()); }

private:
    std::vector<uint32_t> free_;
};

// 虚拟内存门面:CPU 通过它访问内存
class VirtualMemory {
public:
    VirtualMemory(Memory& phys, uint32_t nFrames);

    // 主接口:访问虚拟地址. 触发缺页时自动调用 handler
    uint8_t  read8 (uint32_t vaddr);
    void     write8(uint32_t vaddr, uint8_t v);

    // 缺页 handler(内核态):默认策略 = 分配一帧 + 映射 + 重试
    void     onPageFault(const PageFault& pf);

    Mmu&     mmu() { return mmu_; }
    Tlb&     tlb() { return tlb_; }

    // 打印页表(供 handler 调试用)
    void     dumpPageTable() const;

    // 统计(阶段 ⑭ 总装 dump 用)
    uint64_t nFaults() const { return nFaults_; }
    uint32_t framesFree() const { return alloc_.freeCount(); }

private:
    // 内部翻译流程(TLB + MMU)
    uint32_t translate(uint32_t vaddr, bool write);

    Memory&        phys_;
    FrameAllocator alloc_;
    uint64_t       nFaults_ = 0;
    Mmu            mmu_;
    Tlb            tlb_;
};

}  // namespace mm
