#include "mm/reactor.hpp"
#include "mm/log.hpp"

namespace mm {

void Poller::add(Handle h, std::shared_ptr<IEventHandler> hd, uint32_t it) {
    regs_[h] = {std::move(hd), it};
}
void Poller::modify(Handle h, uint32_t it) {
    auto it2 = regs_.find(h);
    if (it2 != regs_.end()) it2->second.interest = it;
}
void Poller::remove(Handle h) { regs_.erase(h); }

void Poller::simulateReady(Handle h, uint32_t evs) {
    auto it = regs_.find(h);
    if (it == regs_.end()) return;
    // 只保留 handler 感兴趣的位
    uint32_t masked = evs & it->second.interest;
    if (masked) readyQ_.push({h, masked});
}

std::vector<Poller::Ready> Poller::waitReady() {
    ++nWaits_;
    std::vector<Ready> out;
    while (!readyQ_.empty()) {
        out.push_back(readyQ_.front());
        readyQ_.pop();
    }
    nEvents_ += out.size();
    return out;
}

std::shared_ptr<IEventHandler> Poller::handlerOf(Handle h) {
    auto it = regs_.find(h);
    return it == regs_.end() ? nullptr : it->second.handler;
}

void EventLoop::run() {
    while (running_) {
        ++nTicks_;
        auto ready = poller_.waitReady();
        if (ready.empty()) {
            // 真机 epoll_wait 会阻塞;教学环境没就绪事件就退出
            running_ = false;
            break;
        }
        // Dispatcher: 分派到每个 handler
        for (auto& r : ready) {
            auto hd = poller_.handlerOf(r.h);
            if (hd) hd->handleEvent(r.h, r.evs);
        }
    }
}

}  // namespace mm
