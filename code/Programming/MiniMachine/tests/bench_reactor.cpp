#include "mm/reactor.hpp"
#include <cstdio>
#include <chrono>

using namespace mm;

class EchoHandler : public IEventHandler {
public:
    void handleEvent(Handle h, uint32_t evs) override {
        if (evs & EV_READ) {
            ++nRead_;
            // 教学:真实场景在这里 read(fd, buf) + write(fd, buf)
        }
        if (evs & EV_HUP) ++nHup_;
    }
    const char* name() const override { return "echo"; }
    uint64_t reads() const { return nRead_; }
private:
    uint64_t nRead_ = 0, nHup_ = 0;
};

int main() {
    constexpr int kConns   = 1000;
    constexpr int kEventsPerConn = 100;

    Poller poller;
    auto handler = std::make_shared<EchoHandler>();

    // 注册 1000 个 fd
    for (int fd = 0; fd < kConns; ++fd)
        poller.add(fd, handler, EV_READ | EV_HUP);

    // 模拟内核就绪通知
    for (int t = 0; t < kEventsPerConn; ++t)
        for (int fd = 0; fd < kConns; ++fd)
            poller.simulateReady(fd, EV_READ);

    EventLoop loop(poller);
    auto t0 = std::chrono::steady_clock::now();
    loop.run();
    auto t1 = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();

    std::printf("[reactor] conns=%d totalEvents=%llu loopTicks=%llu wallMs=%lld\n",
        kConns, (unsigned long long)handler->reads(),
        (unsigned long long)loop.nTicks(), (long long)ms);
    std::printf("  events/sec ≈ %.0f  (single thread!)\n",
        handler->reads() * 1000.0 / (ms > 0 ? ms : 1));
    std::printf("  poller: waits=%llu events=%llu avgEventsPerWait=%.2f\n",
        (unsigned long long)poller.nWaits(),
        (unsigned long long)poller.nEvents(),
        double(poller.nEvents()) / poller.nWaits());
    return 0;
}
