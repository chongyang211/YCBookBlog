#include <atomic>
#include <thread>
#include <chrono>
#include <cstdio>

// ★ 关键结构:两个变量恰好在同一缓存行
struct Adjacent {
    std::atomic<long> a{0};
    std::atomic<long> b{0};
};

// ★ 用 alignas(64) 强制隔开缓存行
struct Padded {
    alignas(64) std::atomic<long> a{0};
    alignas(64) std::atomic<long> b{0};
};

template <typename S>
static double run(int iters) {
    S s;
    auto worker = [&](std::atomic<long>& x) {
        for (int i = 0; i < iters; ++i) x.fetch_add(1, std::memory_order_relaxed);
    };
    auto t0 = std::chrono::high_resolution_clock::now();
    std::thread t1([&]{ worker(s.a); });
    std::thread t2([&]{ worker(s.b); });
    t1.join(); t2.join();
    auto t3 = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(t3 - t0).count();
}

int main() {
    constexpr int iters = 10'000'000;

    double t1 = run<Adjacent>(iters);
    double t2 = run<Padded>(iters);

    std::printf("┌──────────────────────────────────────────────────────┐\n");
    std::printf("│  ★ 真机 std::atomic 伪共享 (10M ops per thread)      │\n");
    std::printf("├──────────────────────────────────────────────────────┤\n");
    std::printf("│  Adjacent (同一缓存行)   %.2f ms\n", t1);
    std::printf("│  Padded   (alignas(64)) %.2f ms\n", t2);
    std::printf("│  speedup:                %.2f×\n", t1 / t2);
    std::printf("└──────────────────────────────────────────────────────┘\n");
    return 0;
}
