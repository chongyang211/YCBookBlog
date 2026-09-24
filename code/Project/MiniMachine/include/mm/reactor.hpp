#pragma once
#include <cstdint>
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>
#include <queue>

namespace mm {

// 事件类型 (位掩码)
enum EventMask : uint32_t {
    EV_NONE  = 0,
    EV_READ  = 1 << 0,
    EV_WRITE = 1 << 1,
    EV_ERROR = 1 << 2,
    EV_HUP   = 1 << 3,   // 对端关闭
};

// Handle: 简化为一个 int fd
using Handle = int;

// 事件处理器接口
class IEventHandler {
public:
    virtual ~IEventHandler() = default;
    virtual void handleEvent(Handle h, uint32_t evs) = 0;
    virtual const char* name() const { return "handler"; }
};

// Poller: 事件多路分离器 (教学模拟)
// 语义等价于 epoll:注册 fd + 等就绪 + 返回就绪列表
class Poller {
public:
    struct Registration {
        std::shared_ptr<IEventHandler> handler;
        uint32_t interest;   // 感兴趣的事件掩码
    };

    void add(Handle h, std::shared_ptr<IEventHandler> hd, uint32_t interest);
    void modify(Handle h, uint32_t interest);
    void remove(Handle h);

    // 教学接口:模拟"来自内核的就绪通知"
    void simulateReady(Handle h, uint32_t evs);

    // "epoll_wait": 阻塞地等,返回就绪列表 (Handle, events)
    struct Ready { Handle h; uint32_t evs; };
    std::vector<Ready> waitReady();

    // 统计
    uint64_t nWaits()  const { return nWaits_; }
    uint64_t nEvents() const { return nEvents_; }
    size_t   registered() const { return regs_.size(); }

    // 供 dispatcher 使用: 拿 handler
    std::shared_ptr<IEventHandler> handlerOf(Handle h);

private:
    std::unordered_map<Handle, Registration> regs_;
    std::queue<Ready> readyQ_;   // 待返回的就绪事件
    uint64_t nWaits_ = 0, nEvents_ = 0;
};

// EventLoop: "死循环 + waitReady + dispatch" —— libuv / Netty / Redis ae.c 的心脏
class EventLoop {
public:
    explicit EventLoop(Poller& p) : poller_(p) {}

    void run();          // 阻塞循环
    void stop() { running_ = false; }

    Poller& poller() { return poller_; }

    // 统计
    uint64_t nTicks() const { return nTicks_; }

private:
    Poller& poller_;
    bool    running_ = true;
    uint64_t nTicks_ = 0;
};

// 教学:模拟 select 的 O(N) 语义 —— waitReady 时线性扫描所有注册项
// (阶段 ⑬ Step 13.4 可选实现:即使只 1 个 fd 就绪,waitReady 也要遍历全部 N 个 fd)
class SelectPoller {
public:
    void add(Handle h, std::shared_ptr<IEventHandler> hd, uint32_t interest);
    void simulateReady(Handle h, uint32_t evs);
    std::vector<Poller::Ready> waitReady();
};

}  // namespace mm
