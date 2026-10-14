#pragma once

#include <string>    // std::string：商品名
#include <atomic>    // std::atomic<int>：原子库存

// ============================================================================
// 【组件定位】Product —— 商品实体：原子库存 + CAS 循环防超卖（并发精华所在）
// ----------------------------------------------------------------------------
// 【设计思路·教程阶段④的演进史】
//   Step 4.1 裸 int 版：单线程跑起来"完美"；
//   Step 4.2 200 线程压测：亲眼看到 stock = -3 的超卖现场 💥 ——
//     根因："if (stock >= n) stock -= n"是"读-判断-写"三步，
//     线程 A 判断完 stock=1 后被切走，线程 B 也判断通过并扣成 0，
//     A 恢复后再扣 → 负数（同一件商品卖出两次）；
//   Step 4.3 atomic + compare_exchange_weak：判断与扣减合并成一个
//     原子操作 → 万级并发零超卖（benchmark 的 PASS 就是验收）。
// ============================================================================

// 商品：库存用 atomic + CAS 循环，防超卖
class Product {
private:
    int               id;
    std::string       name;
    double            price;
    // 【字段分界】id/name/price 构造后不再变 —— 无需原子保护；
    // stock 是唯一会被多线程"同时改"的字段 → atomic。
    // （这正是"atomic 用于单变量"的取舍：复合状态用锁，见 OrderManager）
    std::atomic<int>  stock;        // 原子库存

public:
    // 初始化列表：name 用 move（参数按值传入，掏空参数省一次拷贝）
    Product(int id_, std::string n, double p, int s)
        : id(id_), name(std::move(n)), price(p), stock(s) {}
    // 【注意】atomic 不可拷贝/移动 → Product 默认也不可拷贝 —— 按引用传递使用

    int getId() const { return id; }
    int getStock() const { return stock.load(); }
    // load()：原子读。即使"只读"，别的线程可能正在 CAS 写它 ——
    // 裸读 atomic<int> 会编译成普通读，撕裂风险取决于平台，load() 显式表达"原子读"
    double getPrice() const { return price; }
    const std::string& getName() const { return name; }   // const 引用零拷贝暴露只读视图

    // CAS 循环扣库存（防超卖）：判断 + 扣减是原子的
    bool tryDeduct(int n) {
        int cur = stock.load();          // 先原子读当前库存作"乐观快照"
        while (cur >= n) {               // 快照够扣才有资格尝试
            // 如果 stock 仍然是 cur，则把它改成 cur - n
            // 失败时 cur 会被自动更新为最新值，进入下一轮
            if (stock.compare_exchange_weak(cur, cur - n)) {
            // 【CAS 语义】compare_exchange_weak(expected, desired)：
            //   原子地比较 stock 与 expected —— 相等则写入 desired 返回 true；
            //   不相等则把 stock 的当前值写回 expected 返回 false。
            // 【为什么是"循环"】CAS 可能失败（别人抢先扣了）：
            //   失败时 cur 已被刷新为最新库存，回到 while 重新判断
            //   "最新值还够不够扣" —— 够就再试，不够就退出循环返回 false。
            //   "乐观并发"：先假设没人抢，失败了重试，而不是先加锁排斥别人。
            // 【为什么是 weak】weak 版允许"值明明相等也谎报失败"
            //   （spurious failure，为 LL/SC 架构优化）—— 循环重试天然
            //   消化这种假失败，weak 比 strong 少几条指令，循环场景首选。
                return true;       // 成功扣减
            }
        }
        return false;             // 库存不够（cur < n）
        // 走到这里 cur 必然 < n（否则还在循环里）—— 扣减失败的唯一出口
    }

    void restock(int n) { stock.fetch_add(n); }
    // fetch_add：原子加法（RMW 单步操作无需 CAS 循环 —— 加法不存在
    // "判断"，任何旧值加 n 都正确），返回旧值（此处不关心）
};
