// box_coro — 10w 用户态协程 benchmark
// 文档：§10 盒 ②
#include "scheduler.hpp"
#include "log.hpp"
#include "timer.hpp"
#include "csv.hpp"
#include "fmt_shim.hpp"
#include <atomic>
#include <deque>

static void bench(int N, std::size_t stack) {
    mlab::Scheduler sched;
    mlab::Scheduler::inst() = &sched;
    std::atomic<int> cnt{0};

    mlab::Timer t;
    for (int i = 0; i < N; ++i)
        sched.co_spawn([&cnt]{ cnt.fetch_add(1); }, stack);
    LOG_INFO("spawned {} coroutines (stack={}B, memory≈{}MB)\n",
             N, stack, (long)N * stack / 1024 / 1024);
    sched.run_until_empty();
    double ms = t.elapsed_ms();
    LOG_OK("done. cnt={}, elapsed={:.1f}ms, {:.0f} coros/sec\n",
           cnt.load(), ms, N / (ms / 1000.0));

    mlab::CsvWriter csv("out/csv/box_coro.csv",
        {"n", "stack_bytes", "elapsed_ms", "throughput_per_sec"}, true);
    csv.row(std::to_string(N), std::to_string(stack),
            fmt::format("{:.2f}", ms),
            fmt::format("{:.0f}", N / (ms / 1000.0)));
}

static void demo_producer_consumer() {
    mlab::Scheduler sched;
    mlab::Scheduler::inst() = &sched;
    std::deque<int> q;

    sched.co_spawn([&]{
        for (int i = 0; i < 5; ++i) {
            q.push_back(i);
            LOG_INFO("produce {}\n", i);
            mlab::co_yield_();
        }
    });
    sched.co_spawn([&]{
        while (true) {
            if (q.empty()) { mlab::co_yield_(); continue; }
            int x = q.front(); q.pop_front();
            LOG_OK("consume {}\n", x);
            if (x == 4) break;
        }
    });
    sched.run_until_empty();
}

int main(int argc, char** argv) {
    if (argc > 1 && std::string(argv[1]) == "demo") {
        demo_producer_consumer();
        return 0;
    }
    int N          = argc > 1 ? std::atoi(argv[1]) : 100000;
    std::size_t sz = argc > 2 ? std::atol(argv[2]) : 2048;
    bench(N, sz);
}
