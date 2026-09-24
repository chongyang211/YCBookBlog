#pragma once
#include "mm/storage.hpp"
#include <array>
#include <cstdint>
#include <memory>
#include <optional>

namespace mm {

// 页表项标志位 (类似 x86 的 PTE)
struct PteFlags {
    bool present : 1 = false;   // 该页是否已分配物理内存
    bool writable: 1 = true;    // 是否可写
    bool user    : 1 = true;    // 是否用户态可访问
    bool dirty   : 1 = false;   // 曾被写过
    bool accessed: 1 = false;   // 曾被访问过
};
// 页表项 = 20 位物理帧号 + 若干标志位 (教学用 uint32_t 存)
struct Pte {
    uint32_t frame : 20 = 0;    // 物理帧号 (乘 4KB = 物理地址)
    uint32_t flags : 12 = 0;    // 位 0=present, 1=writable, 2=user, 3=dirty, 4=accessed
    bool present() const { return flags & 1u; }
    bool writable() const { return flags & 2u; }
    void setPresent(bool v) { flags = v ? (flags | 1u) : (flags & ~1u); }
    void setWritable(bool v){ flags = v ? (flags | 2u) : (flags & ~2u); }
    void setAccessed(bool v){ flags = v ? (flags | 16u): (flags & ~16u); }
    void setDirty  (bool v) { flags = v ? (flags | 8u) : (flags & ~8u); }
    bool accessed() const { return flags & 16u; }
    bool dirty()    const { return flags & 8u; }
};

// 一张页表 (1024 项)
struct PageTable {
    std::array<Pte, 1024> entries{};
};

// 页目录 (1024 项, 每项要么空要么指向一张页表)
struct PageDirectory {
    std::array<std::unique_ptr<PageTable>, 1024> tables;

    // 查/建 (mkdir=true 会为空目录项创建新页表)
    PageTable* getTable(uint32_t dirIdx, bool mkdir);
};

// 缺页异常
struct PageFault {
    uint32_t vaddr;
    bool     write;      // 是读缺页还是写缺页
    const char* reason;  // "not present" / "readonly" / "user-mode denied"
};

// MMU: 地址翻译单元
class Mmu {
public:
    Mmu();

    // 主接口:虚拟 → 物理; 失败抛 PageFault
    uint32_t translate(uint32_t vaddr, bool write);

    // 内核态映射:vaddr → frame
    void mapPage(uint32_t vaddr, uint32_t frame, bool writable = true);

    PageDirectory& pd() { return pd_; }

    // 拆解虚拟地址
    static uint32_t dirIdx (uint32_t v) { return (v >> 22) & 0x3FF; }
    static uint32_t pageIdx(uint32_t v) { return (v >> 12) & 0x3FF; }
    static uint32_t offset (uint32_t v) { return v & 0xFFF; }

private:
    PageDirectory pd_;
};

}  // namespace mm
