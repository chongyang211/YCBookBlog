#pragma once

#include "OrderStrategy.h"
#include "ThreadPool.h"
#include <future>          // std::future<int>
#include <algorithm>       // std::min：裁剪批次右边界

// ============================================================================
// 【组件定位】BatchStrategy —— 批量下单策略：分批提交，组内并发、组间串行
// ----------------------------------------------------------------------------
// 【设计思路·教程 Step 5.3】Flash 的隐患：10 万单 = 10 万个 future 同时
//   挂在内存里（每个 future+packaged_task 都有堆分配）—— 内存压力大。
//   Batch 的解法：每次只提交 batchSize 个，等本批全部完成（future.get）
//   再发下一批 —— "内存峰值 ≤ batchSize × 单任务开销"，用吞吐换内存。
// 【对照】Flash = 全并发（最快、最耗内存）；Normal = 全串行（最省、最慢）；
//   Batch = 中庸之道（分批节流）—— 三者构成并发度的完整谱系。
// ============================================================================

// 批量下单策略：分批提交，组内并发、组间串行，控制内存压力
class BatchStrategy : public OrderStrategy {
private:
    ThreadPool& pool;
    size_t      batchSize;     // 每批的任务数（背压参数：越小内存越省、吞吐越低）

public:
    BatchStrategy(ThreadPool& p, size_t bs = 10) : pool(p), batchSize(bs) {}

    std::vector<int> placeOrders(Product& product, OrderManager& mgr,
                                  int userId,
                                  const std::vector<int>& quantities) override {
        std::vector<int> result(quantities.size(), -1);
        // 【初始化技巧】全部预填 -1：若批次循环因异常中断，
        // "未处理"的单天然是失败标记 —— 不会误报成功（防御性默认值）

        // 每 batchSize 个为一组，组内并发，组间串行
        for (size_t i = 0; i < quantities.size(); i += batchSize) {
            size_t end = std::min(i + batchSize, quantities.size());
            // min 裁剪右边界：最后一批可能不足 batchSize（如 25 单 batch=10 → 10+10+5）

            std::vector<std::future<int>> futs;
            futs.reserve(end - i);                  // 本批的 future 数组（批结束即销毁复用）
            for (size_t j = i; j < end; ++j) {
                int qty = quantities[j];
                // 【必须先取出 j】lambda 按值捕 qty 而非引用捕 j：
                //   捕 &j 的话所有任务共享循环变量 —— 提交后循环继续走，
                //   j 变了，任务执行时读到的是"未来值"（经典悬垂竞态）
                futs.push_back(pool.submit([&product, &mgr, userId, qty]{
                    if (product.tryDeduct(qty)) {
                        return mgr.addOrder(userId, product.getId(), qty);
                    }
                    return -1;
                }));      // 组内并发：本批任务一起进池
            }
            // 等本批完成后再开下一批（背压关键：内存峰值被锁死在 batchSize）
            for (size_t j = i; j < end; ++j) {
                result[j] = futs[j - i].get();
                // futs 从 0 开始编号，result 用绝对下标 j —— 偏移 i 对齐
                // get() 阻塞到本批任务全部完成 → 下一批才允许开始
            }
        }
        return result;
    }
};
