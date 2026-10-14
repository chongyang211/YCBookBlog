#pragma once

#include "OrderStrategy.h"

// ============================================================================
// 【组件定位】NormalStrategy —— 普通下单策略：单线程顺序处理（基准参照系）
// ----------------------------------------------------------------------------
// 【设计思路】最朴素实现：一个 for 循环逐单处理 —— 既是"能用的默认策略"，
//   也是并发策略（Flash/Batch）的性能对照组：同样的下单逻辑，
//   顺序 vs 并发的耗时差就是并发的净收益（教程 Step 5.2 实测 3 倍加速）。
// 【教学价值】它证明"并发不是必须的"：低流量场景顺序版最简单可靠，
//   并发只在吞吐不够时才值得引入复杂度 —— 工程判断力的素材。
// ============================================================================

// 普通下单策略：单线程顺序处理
class NormalStrategy : public OrderStrategy {
public:
    std::vector<int> placeOrders(Product& product, OrderManager& mgr,
                                  int userId,
                                  const std::vector<int>& quantities) override {
        // override：显式标记重写基类纯虚函数，签名不匹配编译期报错
        std::vector<int> orderIds;
        orderIds.reserve(quantities.size());
        // reserve 预分配：push_back 不再触发扩容拷贝（万级下单时省大量搬运）
        for (int qty : quantities) {            // 逐单顺序处理（无任何并发）
            if (product.tryDeduct(qty)) {       // 原子扣库存（够扣才扣）
                // 虽然单线程版用不上 atomic，但复用同一 Product ——
                // 扣减入口统一，切换策略时行为一致
                orderIds.push_back(mgr.addOrder(userId, product.getId(), qty));
                // 成功：登记订单并记录新订单 id
            } else {
                orderIds.push_back(-1);             // 失败标记
                // -1 哨兵：调用方 count(id > 0) 即知成功数（main 里在用）
            }
        }
        return orderIds;
    }
};
