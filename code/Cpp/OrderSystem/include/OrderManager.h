#pragma once

#include "Order.h"          // Order 实体
#include <vector>           // std::vector：订单存储
#include <shared_mutex>     // std::shared_mutex / shared_lock / unique_lock
#include <atomic>           // std::atomic<int>
#include <optional>         // std::optional

// ============================================================================
// 【组件定位】OrderManager —— 订单管理器：存储 + 多线程读写（读写锁的舞台）
// ----------------------------------------------------------------------------
// 【设计思路·教程阶段③的演进史】
//   Step 3.2 mutex 版基线：读和写都用独占锁 —— 查询时把所有并发读也串行化了；
//   Step 3.3 改 shared_mutex：读多写少场景（查询 >> 下单），共享读锁让
//     多个查询同时进行 —— 吞吐翻倍（读吞吐 = 并发数倍提升）。
// 【两把"武器"的分工】
//   shared_lock（读锁）：多个读者可同时持有（互不排斥）；
//   unique_lock（写锁）：独占 —— 持有期间读者/其他写者全被挡（写优先防脏读）。
//   nextId 用 atomic：发号是纯单变量自增，CAS/锁都行，atomic 最轻。
// ============================================================================

// 订单管理器：订单存储 + 多线程读写
// - 写（addOrder）用独占锁，读（findOrder/ordersByUser/size）用共享锁
// - 订单 ID 用 atomic 自增，避免两个线程拿到同一 ID
class OrderManager {
private:
    std::vector<Order>           orders;
    // 【为什么 vector 而非 map】按 id 查找是线性扫（findOrder）——
    // 万级数据顺序扫已经够快（缓存友好），教学版选最简容器；
    // 真要按 id 高频查，换 unordered_map<int, Order> + 读写锁。
    mutable std::shared_mutex    mtx;            // 读写锁
    // mutable：const 成员函数（读操作）里也要拿（共享）锁 —— 锁本身要可变
    std::atomic<int>             nextId{1};      // ID 自增用 atomic
    // 【为什么不用 mtx 保护 nextId】发号与"插入列表"是两件事：
    // fetch_add 一条原子指令搞定发号，不必卷入读写锁 ——
    // "锁的粒度最小化"：不相关的状态用不同的同步原语。

public:
    int addOrder(int userId, int productId, int qty) {
        Order o(nextId.fetch_add(1), userId, productId, qty);
        // 【发号时机】先原子取号再争锁 —— 即使两个线程同时下单，
        // fetch_add 保证各拿各的号，绝无重复 ID（这就是注释里说的
        // "避免两个线程拿到同一 ID"）。
        std::unique_lock lock(mtx);              // 写：独占锁
        // unique_lock（非 std::lock_guard）：shared_mutex 的写锁语义；
        // CTAD（C++17）：模板参数从 mtx 推导为 unique_lock<shared_mutex>
        orders.push_back(std::move(o));
        return o.id;
        // 【细节】o 已被 move 进容器，但 move 的是 string 等资源，
        // 基本类型字段（id）仍在栈对象里可读 —— 返回 o.id 安全
    }

    std::optional<Order> findOrder(int id) const {
        std::shared_lock lock(mtx);              // 读：共享锁
        // shared_lock：多线程可同时进到这里查 —— 读锁互不排斥
        for (const auto& o : orders) {
            if (o.id == id) return o;            // 命中：返回订单拷贝（optional 装值）
        }
        return std::nullopt;                     // 未找到：optional 空态
        // 【optional 的价值】"查无此单"是常态而非异常 —— 返回类型显式
        // 表达"可能没有"，调用方必须处理（对比裸指针的 null 歧义）
    }

    std::vector<Order> ordersByUser(int userId) const {
        std::shared_lock lock(mtx);              // 读锁
        std::vector<Order> result;
        for (const auto& o : orders) {
            if (o.userId == userId) result.push_back(o);   // 逐条拷贝命中订单
        }
        return result;       // 拷贝出去（在锁内构建快照，锁外使用 —— 无锁依赖）
    }

    size_t size() const {
        std::shared_lock lock(mtx);
        return orders.size();
    }
};
