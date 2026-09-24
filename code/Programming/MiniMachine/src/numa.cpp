#include "mm/numa.hpp"
#include "mm/log.hpp"

namespace mm {

NumaSystem::NumaSystem() : nodes_(kNumNodes) {}

uint8_t NumaSystem::read8(int coreId, uint32_t addr) {
    int coreNode = nodeOfCore(coreId);
    int addrNode = nodeOfAddr(addr);
    uint32_t localAddr = addr % kMemPerNode;
    uint8_t v = nodes_[addrNode].read8(localAddr);
    if (coreNode == addrNode) { ++localAcc_;  totalCyc_ += kLocalCycles; }
    else                       { ++remoteAcc_; totalCyc_ += kRemoteCycles;
                                 MM_TRACE("numa", "★ cross-node: core%d(N%d) → addr 0x%X(N%d)",
                                     coreId, coreNode, addr, addrNode); }
    return v;
}

void NumaSystem::write8(int coreId, uint32_t addr, uint8_t v) {
    int coreNode = nodeOfCore(coreId);
    int addrNode = nodeOfAddr(addr);
    uint32_t localAddr = addr % kMemPerNode;
    nodes_[addrNode].write8(localAddr, v);
    if (coreNode == addrNode) { ++localAcc_;  totalCyc_ += kLocalCycles; }
    else                       { ++remoteAcc_; totalCyc_ += kRemoteCycles; }
}

}  // namespace mm
