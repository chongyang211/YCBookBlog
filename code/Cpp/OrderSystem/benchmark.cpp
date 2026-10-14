// ============================================================================
// 【文件定位】benchmark.cpp —— 万级并发压测 + 一致性校验（整个项目的"终审"）
// ----------------------------------------------------------------------------
// 【设计思路·教程阶段⑦ Step 7.2】并发代码"看起来对"不算对 —— 必须用
//   压测暴露竞态、用守恒校验证明无超卖无漏单：
//   1000 个用户线程 × 每人 10 单 = 10000 请求，库存只有 5000 ——
//   一半请求注定失败，正好检验"失败路径"的干净程度。
// 【验收公式】成功订单数 + 剩余库存 == 初始库存
//   超卖 → 成功 > 5000（左式 > 右式）；漏单/丢扣 → 左式 < 右式。
//   PASS 是全项目（CAS 防超卖、读写锁、线程池）的集体证词。
// ============================================================================

#include "include/ThreadPool.h"
#include "include/OrderManager.h"
#include "include/Product.h"
#include "include/FlashStrategy.h"
#include <chrono>      // steady_clock：压测定时
#include <iostream>
#include <atomic>      // std::atomic<int>：跨线程计数器
#include <thread>      // std::thread：1000 个"用户线程"

// 最终验收：万级并发压测 + 一致性校验
int main() {
    constexpr int TOTAL_USERS     = 1000;    // 用户线程数
    constexpr int ORDERS_PER_USER = 10;      // 每人下 10 单
    constexpr int INITIAL_STOCK   = 5000;        // 故意 < 总请求 10000
    // 【库存为何故意不足】5000 件对 10000 单：一半请求必须失败 ——
    // 如果库存管够，"超卖"永远不会被发现（扣到负数才暴露）；
    // 供不应求才是对 CAS 最严酷的考验

    ThreadPool   pool(std::thread::hardware_concurrency());   // 工人 = 核数
    OrderManager mgr;
    Product      iphone(1, "iPhone 15", 7999.0, INITIAL_STOCK);

    FlashStrategy flash(pool);                 // 压测用全并发策略（最激进）
    std::vector<int> qtys(ORDERS_PER_USER, 1); // 每人 10 单、每单 1 件
    std::atomic<int> totalSuccess{0};
    // atomic 计数器：1000 个线程同时 += —— 裸 int 会丢计数（非原子 RMW）

    auto t0 = std::chrono::steady_clock::now();
    // steady_clock（单调时钟）：不受系统时间回拨影响 —— 计时基准的正确选择
    // （system_clock 被改时间点会算出负耗时）

    // 启动 1000 个用户线程，每人下 10 单
    std::vector<std::thread> users;
    users.reserve(TOTAL_USERS);                // 预分配：避免 1000 次扩容
    for (int u = 0; u < TOTAL_USERS; ++u) {
        users.emplace_back([&, u]{
            // 捕获设计：& 全引用捕（pool/mgr/iphone/qtys/flash 都活得比线程久），
            // u 按值捕 —— 循环变量按值拷贝快照，线程各拿各的用户号
            auto ids = flash.placeOrders(iphone, mgr, u, qtys);
            // 每个用户线程内部又向池提交 10 个任务 —— 两级并发：
            // 用户线程（等 future）× 工人线程（抢库存）
            for (int id : ids) if (id > 0) totalSuccess++;
            // 原子自增统计本用户的成功单数（fetch_add 循环展开的形式）
        });
    }
    for (auto& t : users) t.join();      // 等全部用户线程完成（join 前不算总账）

    auto t1 = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();

    int total    = TOTAL_USERS * ORDERS_PER_USER;
    int success  = totalSuccess.load();         // 原子读最终计数
    int leftover = iphone.getStock();           // 原子读剩余库存

    std::cout << "========================================\n";
    std::cout << "总请求数:    " << total << "\n";
    std::cout << "成功订单:    " << success << "\n";
    std::cout << "剩余库存:    " << leftover << "\n";
    std::cout << "总耗时:     " << ms << " ms\n";
    std::cout << "QPS:        " << (total * 1000.0 / ms) << "\n";
    // QPS = 请求数 × 1000 / 毫秒耗时（ms 为 0 会除零 —— 万级压测下不实际发生）

    // 关键一致性验证：成功 + 剩余 == 初始（零超卖、零漏单）
    bool consistent = (success + leftover == INITIAL_STOCK);
    // 【守恒律】库存的每一次成功扣减 ↔ 恰好一张订单：
    //   超卖（卖了 5001 件）→ success + leftover > INITIAL_STOCK 不可能，
    //   实际表现是 success = 5001 > 库存上限 —— 等式被破坏 → FAIL；
    //   漏单（扣了库存没记单）→ success 偏小 → 等式同样破坏。
    //   等式成立 = CAS 扣减与订单登记严格一一对应
    std::cout << "一致性校验: 成功(" << success << ") + 剩余(" << leftover
              << ") == 初始(" << INITIAL_STOCK << ") ? "
              << (consistent ? "PASS" : "FAIL：超卖或漏单！") << "\n";
    std::cout << "========================================\n";
    return consistent ? 0 : 1;          // 一致则退出码 0
    // 【退出码设计】0/1 是给 CI 脚本的机器信号：压测进流水线，
    // 回归（引入竞态）会直接让构建红掉 —— 人看的 PASS 只是副产品
}
