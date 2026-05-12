// stress.cpp —— 多线程压测，验证 Store 的并发安全
//
// 跑法：
//   ./build/mini_kv_stress
// 期望：
//   - 不崩溃
//   - 输出 final size = 100（k0 .. k99）
//   - ops/s 数值（与机器相关）

#include "mkv/store.hpp"
#include "mkv/value.hpp"

#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

int main() {
    mkv::Store store;
    constexpr int N = 8;        // 线程数
    constexpr int M = 100000;   // 每线程操作次数

    auto t0 = std::chrono::steady_clock::now();
    {
        std::vector<std::jthread> ths;
        for (int i = 0; i < N; ++i) {
            ths.emplace_back([&store, i] {
                for (int j = 0; j < M; ++j) {
                    auto k = "k" + std::to_string(j % 100);
                    store.set(k, static_cast<std::int64_t>(i));
                    (void)store.get(k);
                }
            });
        }
        // ths 析构时自动 join 所有 jthread
    }
    auto t1 = std::chrono::steady_clock::now();

    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    auto ops = static_cast<long long>(N) * M * 2;
    std::cout << ops << " ops in " << ms << " ms = "
              << (ms == 0 ? 0.0 : (static_cast<double>(ops) / static_cast<double>(ms) * 1000.0))
              << " ops/s\n";
    std::cout << "final size = " << store.size() << "\n";
    return 0;
}
