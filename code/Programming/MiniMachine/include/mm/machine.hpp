#pragma once
// MiniMachine 总装头文件 (阶段 ⑭)
// Component 基类在 mm/component.hpp —— 独立成头, 断开循环包含。
#include "mm/component.hpp"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "mm/memory.hpp"
#include "mm/cache.hpp"
#include "mm/vm.hpp"
#include "mm/cpu.hpp"
#include "mm/bus.hpp"
#include "mm/io.hpp"
#include "mm/dma.hpp"
#include "mm/ivt.hpp"

namespace mm {

// 🎓 迷你冯诺依曼机总装 (阶段 ⑭)
// 聚合并连线前 13 阶段的所有部件: 构造顺序 = 依赖顺序
class Machine {
public:
    Machine();

    // ---- 阶段 ⑭ API: 程序直载 ----
    // 加载 masm 生成的指令序列到内存起始地址
    void load(const std::vector<uint32_t>& program, uint32_t base = 0);
    // 复位到 entryPc,准备执行
    void reset(uint32_t entryPc = 0);
    // 运行,直到 HALT 或 maxCycles 终止
    void run(uint64_t maxCycles = 100000);
    // dump 大表:显示各阶段部件的统计
    void dump();

    // ---- REPL API ----
    void cmdLoad(const std::string& path);   // 读 masm 产出的 .bin
    void cmdRun()  { run(); }
    void cmdStep() { (void)cpu_.step(); }
    void cmdDump() { dump(); }

    // ---- 部件访问 (供 REPL / 测试 / 毕业答辩用) ----
    Cpu&           cpu()    { return cpu_; }
    Memory&        memory() { return memory_; }
    VirtualMemory& vm()     { return vm_; }
    SystemBus&     bus()    { return bus_; }
    IoController&  io()     { return io_; }
    DmaEngine&     dma()    { return dma_; }
    Ivt&           ivt()    { return ivt_; }
    Cache&         l1()     { return l1_; }
    Cache&         l2()     { return l2_; }
    Cache&         l3()     { return l3_; }

    bool     halted()    const { return halted_; }
    uint64_t ranCycles() const { return ranCycles_; }

private:
    // ── 存储层次 (阶段 ②⑥): 声明顺序 = 构造顺序, 逐层引用下一层 ──
    Memory       memory_;
    Cache        l3_;
    Cache        l2_;
    Cache        l1_;

    // ── 虚拟内存 (阶段 ⑨): 内部持有 Mmu + Tlb + FrameAllocator ──
    VirtualMemory vm_;

    // ── CPU (阶段 ⑤) ──
    Cpu          cpu_;

    // ── 总线 & IO (阶段 ⑩ ⑪) ──
    SystemBus    bus_;
    IoController io_;
    DmaEngine    dma_;

    // ── 异常 (阶段 ⑫) ──
    Ivt          ivt_;

    bool         halted_ = false;
    uint64_t     ranCycles_ = 0;

    void wireSyscallHandlers();
    void wireExceptionHandlers();
};

}  // namespace mm
