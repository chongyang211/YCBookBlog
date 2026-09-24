#pragma once
#include "mm/memory.hpp"
#include <vector>

namespace mm {

// 两 socket NUMA 系统:2 个 node,每个 node 有 4 个 core + 32KB 内存
class NumaSystem {
public:
    static constexpr int      kNumNodes   = 2;
    static constexpr int      kCoresPerNode = 4;
    static constexpr uint32_t kMemPerNode  = 32 * 1024;
    static constexpr uint32_t kLocalCycles = 100;
    static constexpr uint32_t kRemoteCycles = 130;  // 30% 惩罚

    NumaSystem();

    // 核 coreId 访问虚拟内存 addr (跨 node 范围: node0 = 0..32K, node1 = 32K..64K)
    uint8_t read8 (int coreId, uint32_t addr);
    void    write8(int coreId, uint32_t addr, uint8_t v);

    static int nodeOfCore(int coreId) { return coreId / kCoresPerNode; }
    static int nodeOfAddr(uint32_t addr) { return addr / kMemPerNode; }

    // 统计
    uint64_t localAccess() const { return localAcc_; }
    uint64_t remoteAccess() const { return remoteAcc_; }
    uint64_t totalCycles() const { return totalCyc_; }

private:
    std::vector<Memory> nodes_;
    uint64_t localAcc_ = 0, remoteAcc_ = 0, totalCyc_ = 0;
};

}  // namespace mm
