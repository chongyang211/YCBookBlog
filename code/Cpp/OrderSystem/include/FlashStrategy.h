#pragma once

#include "OrderStrategy.h"
#include "ThreadPool.h"    // 依赖注入的线程池
#include <future>          // std::future<int>：异步收集每单结果

// ============================================================================
// 【组件定位】FlashStrategy —— 秒杀下单策略：每单一个 task 全并发抢库存
// ----------------------------------------------------------------------------
// 【设计思路·教程 Step 5.2】秒杀场景特征：瞬时海量请求、库存远少于请求。
//   把每一单独立提交线程池 —— 工人线程们并发调 tryDeduct，
//   "谁先 CAS 成功谁抢到"，抢不到的拿到 -1 —— 并发公平竞争的具象化。
// 【依赖注入】pool 是外部传入的引用而非自建 ——
//   ① 多策略共享一个池（main 里 Flash/Batch 共用 pool(4)）；
//   ② 生命周期归调用方管（main 结束时统一 shutdown）——
//   策略自己不持有资源，"瘦组件"更可组合。
// ============================================================================

// 秒杀下单策略：每个下单作为一个 task 提交线程池，工人并发抢库存
class FlashStrategy : public OrderStrategy {
private:
    ThreadPool& pool;
    // 引用成员：策略不拥有线程池，只"借用"（依赖注入的核心约定）

public:
    explicit FlashStrategy(ThreadPool& p) : pool(p) {}
    // explicit：单参构造防隐式转换（别让 ThreadPool 悄悄变 FlashStrategy）

    std::vector<int> placeOrders(Product& product, OrderManager& mgr,
                                  int userId,
                                  const std::vector<int>& quantities) override {
        std::vector<std::future<int>> futs;
        futs.reserve(quantities.size());    // 预分配 future 数组
        for (int qty : quantities) {
            // 逐单提交：每单一个独立任务（全并发语义）
            futs.push_back(pool.submit([&product, &mgr, userId, qty]{
                // 【捕获列表设计】
                //   &product/&mgr 按引用捕：对象在外部（main 栈上），
                //     按值拷贝 Product 会拷不动（atomic 成员不可拷贝）
                //     且改副本无意义 —— 必须引用本体；
                //   userId/qty 按值捕：int 轻小，且 lambda 生命周期
                //     可能超出循环变量 —— 值拷贝防悬垂。
                if (product.tryDeduct(qty)) {           // CAS 抢库存（并发核心）
                    return mgr.addOrder(userId, product.getId(), qty);   // 抢到：登记订单
                }
                return -1;                               // 没抢到：失败哨兵
            }));
        }

        // 收集所有结果（按提交顺序）
        std::vector<int> result;
        result.reserve(futs.size());
        for (auto& f : futs) result.push_back(f.get());
        // 【future::get 的两重作用】① 阻塞等该单完成取结果；
        // ② 任务里抛的异常在这里重抛（packaged_task 的异常通道）。
        // 顺序遍历 = 按提交顺序对齐结果（契约：第 i 个 future 对应第 i 单）
        return result;
    }
};
