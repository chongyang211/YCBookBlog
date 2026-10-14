// ============================================================================
// 【文件定位】stress.cpp —— 多线程压测：8 线程 × 10 万次混合读写
// ----------------------------------------------------------------------------
// 【验收思路】并发代码"看起来对"不算数 —— 压测暴露竞态：
//   8 线程对同一批 key 疯狂 SET/GET，shared_mutex 若有漏洞这里必崩/丢数据。
// 【两个观测指标】ops/s（吞吐：读写锁 vs 独占锁的差距现场）与
//   final size（正确性：100 个不同 key 最终恰好 100 个 —— 无重复无丢失）。
// ============================================================================

// 多线程压测：8 线程 × 10 万次混合读写，验证 shared_mutex 扛住并发
#include <iostream>
#include <vector>
#include <thread>      // std::jthread（C++20）
#include <chrono>     // steady_clock：压测定时（单调时钟不受改时间影响）
#include <string>

#include "mkv/store.hpp"

using namespace mkv;

int main() {
    Store store;
    std::vector<std::jthread> ths;
    // jthread 向量：出作用域逐个析构（自动 request_stop+join）——
    // 下面 ths.clear() 提前触发同样效果
    constexpr int N = 8;       // 线程数（jthread 无 stop_token 参数也可：
    constexpr int M = 100000;  // 每线程操作数  不写参数就不注入，纯计算型线程）

    auto t0 = std::chrono::steady_clock::now();    // 压测起点
    for (int i = 0; i < N; ++i) {
        ths.emplace_back([&store, i] {
            // 捕获设计：store 按引用（共享同一仓库），i 按值（线程各存各的号）
            for (int j = 0; j < M; ++j) {
                // j % 100：8 线程挤在 100 个 key 上互踩 —— 制造真实竞争压力
                store.set("k" + std::to_string(j % 100), static_cast<std::int64_t>(i));
                // 写：把线程号存进去（后写覆盖先写 —— 顺序无关紧要，
                //     重要的是"不崩、不产生非法状态"）
                (void)store.get("k" + std::to_string(j % 100));
                // 读：(void) 丢弃结果 —— 本测试读只为制造读锁竞争流量
            }
        });
    }
    ths.clear();    // join all
    // 【clear 的妙用】清空向量 → 逐个析构 jthread → 自动 join 全部线程 ——
    // 一行完成"等待所有线程结束"（对比裸 thread 要手写 for 循环 join）
    auto t1 = std::chrono::steady_clock::now();

    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    std::cout << N * M * 2 << " ops in " << ms << " ms = "
              << (N * M * 2.0 / ms * 1000) << " ops/s\n";
    // 每轮 1 set + 1 get = 2 ops —— 总操作数 ×2；ops/s = 总数/毫秒×1000
    std::cout << "final size = " << store.size() << "\n";
    // 正确性断言（肉眼版）：应为 100 —— 8 线程 × 相同 100 个 key，
    // 哈希表无重复无丢失即证明并发安全（若 size ≠ 100，锁已出事）
    return 0;
}
