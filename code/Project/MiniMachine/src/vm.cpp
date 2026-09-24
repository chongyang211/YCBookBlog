#include "mm/vm.hpp"
#include "mm/log.hpp"
#include <cstdio>
#include <stdexcept>

namespace mm {

// ---- FrameAllocator ----
FrameAllocator::FrameAllocator(uint32_t n) {
    free_.reserve(n);
    // 保留物理帧 0 (页表使用 vaddr 0 时能识别成"未映射")
    for (uint32_t i = n; i-- > 1; ) free_.push_back(i);
}
uint32_t FrameAllocator::alloc() {
    if (free_.empty()) throw std::runtime_error("OOM: no free frame");
    uint32_t f = free_.back();
    free_.pop_back();
    return f;
}
void FrameAllocator::free(uint32_t f) { free_.push_back(f); }

// ---- VirtualMemory ----
VirtualMemory::VirtualMemory(Memory& phys, uint32_t nFrames)
    : phys_(phys), alloc_(nFrames) {}

uint32_t VirtualMemory::translate(uint32_t vaddr, bool write) {
    uint32_t vpage = vaddr >> 12;
    if (auto f = tlb_.lookup(vpage); f) {
        return (*f << 12) | (vaddr & 0xFFF);
    }
    // TLB miss → 走 MMU
    uint32_t paddr = mmu_.translate(vaddr, write);   // 可能抛 PageFault
    tlb_.insert(vpage, paddr >> 12);
    return paddr;
}

uint8_t VirtualMemory::read8(uint32_t vaddr) {
    for (int retry = 0; retry < 2; ++retry) {
        try {
            uint32_t paddr = translate(vaddr, /*write=*/false);
            return phys_.read8(paddr);
        } catch (const PageFault& pf) {
            if (retry > 0) throw;    // 二次缺页说明 handler 没修好
            onPageFault(pf);
        }
    }
    throw std::runtime_error("unreachable");
}

void VirtualMemory::write8(uint32_t vaddr, uint8_t v) {
    for (int retry = 0; retry < 2; ++retry) {
        try {
            uint32_t paddr = translate(vaddr, /*write=*/true);
            phys_.write8(paddr, v);
            return;
        } catch (const PageFault& pf) {
            if (retry > 0) throw;
            onPageFault(pf);
        }
    }
}

// 默认缺页处理:分配一帧 + 映射 + TLB 失效
void VirtualMemory::onPageFault(const PageFault& pf) {
    ++nFaults_;
    MM_WARN("mmu", "★ Page Fault @ V=0x%08X (%s, %s)",
        pf.vaddr, pf.write ? "write" : "read", pf.reason);
    // 若是权限错误(readonly 写),不是"未映射",不能通过分配帧修复
    if (std::string(pf.reason) == "readonly") throw pf;

    uint32_t frame = alloc_.alloc();
    uint32_t vpage = pf.vaddr & ~0xFFFu;
    mmu_.mapPage(vpage, frame, /*writable=*/true);
    tlb_.invalidate(vpage >> 12);
    MM_INFO("mmu", "  → allocated frame %u, remapped V=0x%08X",
        frame, vpage);
}

void VirtualMemory::dumpPageTable() const {
    std::puts("┌─── PageTable Dump ────────────────────────────────┐");
    // 用 const_cast 借用非 const 版本(dumping only)
    auto& pd = const_cast<Mmu&>(mmu_).pd();
    for (size_t di = 0; di < 1024; ++di) {
        if (!pd.tables[di]) continue;
        std::printf("│  Dir[%zu] → table @ %p\n", di, (void*)pd.tables[di].get());
        for (size_t pi = 0; pi < 1024; ++pi) {
            const Pte& e = pd.tables[di]->entries[pi];
            if (!e.present()) continue;
            uint32_t vaddr = static_cast<uint32_t>((di << 22) | (pi << 12));
            std::printf("│    V=0x%08X → P=0x%08X  [%s%s%s%s]\n",
                vaddr, e.frame << 12,
                e.writable() ? "W" : "-",
                e.accessed() ? "A" : "-",
                e.dirty()    ? "D" : "-",
                "P");
        }
    }
    std::puts("└───────────────────────────────────────────────────┘");
}

}  // namespace mm
