// 多线程压测：8 线程 × 10 万次混合读写，验证 shared_mutex 扛住并发
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <string>

#include "mkv/store.hpp"

using namespace mkv;

int main() {
    Store store;
    std::vector<std::jthread> ths;
    constexpr int N = 8;
    constexpr int M = 100000;

    auto t0 = std::chrono::steady_clock::now();
    for (int i = 0; i < N; ++i) {
        ths.emplace_back([&store, i] {
            for (int j = 0; j < M; ++j) {
                store.set("k" + std::to_string(j % 100), static_cast<std::int64_t>(i));
                (void)store.get("k" + std::to_string(j % 100));
            }
        });
    }
    ths.clear();    // join all
    auto t1 = std::chrono::steady_clock::now();

    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    std::cout << N * M * 2 << " ops in " << ms << " ms = "
              << (N * M * 2.0 / ms * 1000) << " ops/s\n";
    std::cout << "final size = " << store.size() << "\n";
    return 0;
}
