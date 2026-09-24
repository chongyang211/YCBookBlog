#include "mm/bus.hpp"
#include "mm/log.hpp"
#include <cassert>

namespace mm {

// -------- 轮询仲裁 --------
int RoundRobinArbiter::next(const std::vector<int>& pending) {
    if (pending.empty()) return -1;
    // 找到 > last_ 的最小 id;若没有则回绕
    int best = -1;
    for (int id : pending) if (id > last_) { best = id; break; }
    if (best == -1) best = pending.front();
    last_ = best;
    return best;
}

// -------- 优先级仲裁 --------
int PriorityArbiter::next(const std::vector<int>& pending) {
    if (pending.empty()) return -1;
    int best = pending[0];
    for (int id : pending) if (id < best) best = id;
    return best;
}

// -------- SystemBus --------
SystemBus::SystemBus(std::unique_ptr<IArbiter> arb) : arb_(std::move(arb)) {}

BusSlave* SystemBus::findSlave(uint32_t addr) {
    for (auto& s : slaves_)
        if (addr >= s.base && addr < s.base + s.size) return &s;
    return nullptr;
}

void SystemBus::request(Request r) {
    if (queues_.size() <= static_cast<size_t>(r.masterId))
        queues_.resize(r.masterId + 1);
    r.issuedAt = clk_;
    queues_[r.masterId].push(r);
}

SystemBus::ServeInfo SystemBus::tick() {
    ++clk_;
    // 收集当前有待处理请求的 master
    std::vector<int> pending;
    for (size_t i = 0; i < queues_.size(); ++i)
        if (!queues_[i].empty()) pending.push_back(static_cast<int>(i));
    int m = arb_->next(pending);
    if (m < 0) return {-1, 0};

    auto& q = queues_[m];
    Request r = q.front(); q.pop();
    BusSlave* s = findSlave(r.addr);
    uint64_t cyc = 1;   // 仲裁 + 传输本身 1 拍
    if (s) {
        // 委托给 slave 的 Storage 读写 (Storage 内部会自己累加它的 cycles)
        if (r.write) s->store->write8(r.addr - s->base, static_cast<uint8_t>(r.value));
        else         (void)s->store->read8(r.addr - s->base);
        // 简化:slave 内部的延迟不算入总线延迟,这里只算总线本身
    }
    clk_ += cyc;
    uint64_t lat = clk_ - r.issuedAt;
    totalLat_ += lat;
    if (lat > maxLat_) maxLat_ = lat;
    ++nServed_;
    return {m, cyc};
}

}  // namespace mm
