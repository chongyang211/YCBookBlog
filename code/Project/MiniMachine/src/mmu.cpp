#include "mm/mmu.hpp"
#include "mm/log.hpp"

namespace mm {

PageTable* PageDirectory::getTable(uint32_t dirIdx, bool mkdir) {
    if (!tables[dirIdx]) {
        if (!mkdir) return nullptr;
        tables[dirIdx] = std::make_unique<PageTable>();
    }
    return tables[dirIdx].get();
}

Mmu::Mmu() = default;

uint32_t Mmu::translate(uint32_t vaddr, bool write) {
    PageTable* pt = pd_.getTable(dirIdx(vaddr), /*mkdir=*/false);
    if (!pt) throw PageFault{vaddr, write, "page-table missing"};
    Pte& pte = pt->entries[pageIdx(vaddr)];
    if (!pte.present())              throw PageFault{vaddr, write, "not present"};
    if (write && !pte.writable())    throw PageFault{vaddr, write, "readonly"};
    // 更新 accessed / dirty 位
    pte.setAccessed(true);
    if (write) pte.setDirty(true);
    return (pte.frame << 12) | offset(vaddr);
}

void Mmu::mapPage(uint32_t vaddr, uint32_t frame, bool writable) {
    PageTable* pt = pd_.getTable(dirIdx(vaddr), /*mkdir=*/true);
    Pte& pte = pt->entries[pageIdx(vaddr)];
    pte.frame = frame;
    pte.setPresent(true);
    pte.setWritable(writable);
    MM_INFO("mmu", "map V=0x%08X → P=0x%08X (%s)",
        vaddr, frame << 12, writable ? "rw" : "ro");
}

}  // namespace mm
