#pragma once
#include "mm/storage.hpp"
#include <memory>
#include <queue>
#include <vector>

namespace mm {

// 仲裁器接口:决定当前拍哪个 master 拿到总线
class IArbiter {
public:
    virtual ~IArbiter() = default;
    // 输入:pending master ids (未完成请求的 master); 输出:被授权的 id
    virtual int next(const std::vector<int>& pending) = 0;
    virtual const char* name() const = 0;
};

// 轮询:上次是谁,这次给下一个
class RoundRobinArbiter : public IArbiter {
public:
    int next(const std::vector<int>& pending) override;
    const char* name() const override { return "RR"; }
private:
    int last_ = -1;
};

// 优先级:id 小的优先(0 最高)
class PriorityArbiter : public IArbiter {
public:
    int next(const std::vector<int>& pending) override;
    const char* name() const override { return "PRI"; }
};

// 简化的 slave 描述:一段物理地址范围 + 一个 Storage
struct BusSlave {
    uint32_t base;
    uint32_t size;
    Storage* store;
};

// 总线本体
class SystemBus {
public:
    explicit SystemBus(std::unique_ptr<IArbiter> arb);

    void addSlave(BusSlave s) { slaves_.push_back(s); }

    // 每个 master 挂队列
    struct Request {
        int      masterId;
        uint32_t addr;
        bool     write;
        uint32_t value;      // write 用
        uint64_t issuedAt;   // 发起拍
    };
    void request(Request r);

    // 一次总线仲裁 + 服务
    // 返回 (被服务的 master, 服务耗时拍数)
    struct ServeInfo { int masterId; uint64_t cycles; };
    ServeInfo tick();

    // 统计
    uint64_t nServed()   const { return nServed_; }
    uint64_t maxLatency() const { return maxLat_; }
    double   avgLatency() const {
        return nServed_ ? double(totalLat_) / nServed_ : 0.0;
    }

    // 内部:找 slave
    BusSlave* findSlave(uint32_t addr);

private:
    std::unique_ptr<IArbiter> arb_;
    std::vector<BusSlave>     slaves_;
    // 每个 master 一个 FIFO
    std::vector<std::queue<Request>> queues_;
    uint64_t clk_ = 0;
    uint64_t nServed_ = 0, totalLat_ = 0, maxLat_ = 0;
};

}  // namespace mm
