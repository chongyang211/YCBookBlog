#pragma once

#include <shared_mutex>    // std::shared_mutex（C++17 读写锁）
#include <string>          // std::string（key 类型）
#include <vector>          // std::vector（keys() 返回）
#include <cstdint>         // std::int64_t / std::size_t

#include "entry.hpp"       // Entry / EntryPtr / IndexMap / Value

namespace mkv {

// ============================================================================
// 【模块定位】store.hpp —— Store：核心数据仓库（并发安全的唯一入口）
// ----------------------------------------------------------------------------
// 【设计思想·教程§10 灵魂三问】
//   ① 为什么不用 atomic？idx_ 是 unordered_map（复合结构），atomic 只保
//     单变量 —— 哈希桶的插入/删除要保护的是"结构不变量"，必须上锁；
//   ② 为什么 shared_mutex？KV 负载读多写少（GET:SET ≈ 10:1）——
//     共享读锁让 GET 并发，写锁独占防脏读，压测可上百万 ops/s；
//   ③ 接口即契约：8 个方法对应 8 个命令的底层语义，命令层只见方法不见锁
//     —— 锁的粒度、类型全部封装在实现里（store.cpp），改锁不动调用方。
// ============================================================================

class Store {
public:
    Store() = default;
    // 默认构造足够：idx_ 空哈希表 + mu_ 未锁定，成员自动初始化

    // 五法则：Store 是有所有权的资源管理者，禁止拷贝
    // 【技术原理】锁不可拷贝（拷贝锁语义无意义）+ 哈希表拷贝代价大 ——
    //   四个 = delete 把拷贝/移动全部显式禁掉：
    //   任何"不小心按值传 Store"的代码直接编译失败（防呆于编译期）
    Store(const Store&)            = delete;
    Store& operator=(const Store&) = delete;
    Store(Store&&)                 = delete;
    Store& operator=(Store&&)      = delete;

    // ====== 命令对应的底层操作 ======
    // 【签名设计通则】key 一律 const std::string&（只读引用零拷贝）；
    //   返回值"语义化"：指针判空 / 计数 / 哨兵值，调用方无需再解释。
    void          set     (const std::string& key, Value v);
    // v 按值收：set 内部 move 进 Entry —— 调用方传左值一次拷贝、右值零拷贝
    EntryPtr      get     (const std::string& key) const;          // 不存在返回 nullptr
    // 返回 EntryPtr（而非 Value）：调用方还要读 TTL/版本等元数据；
    // nullptr = "逻辑上不存在"（物理不存在或已过期，统一口径）
    std::size_t   del     (const std::string& key);                // 删了返回 1，否则 0
    bool          expire  (const std::string& key, int seconds);   // 设过期；key 不存在返回 false
    std::int64_t  ttl     (const std::string& key) const;          // 剩余秒；-2 不存在 -1 永久
    // TTL 哨兵约定（Redis 同款）：-2 不存在 / -1 永久 / >=0 剩余秒 ——
    // 一个 int64 表达三态，调用方 switch 即可
    std::vector<std::string> keys() const;
    // 返回快照拷贝（锁内构建，锁外使用 —— 不持有锁的引用回到调用方）
    std::size_t   size() const noexcept;

    // ====== 维护操作 ======
    void          purgeExpired();   // 后台线程调用：扫描并删除过期 key

private:
    mutable std::shared_mutex mu_;   // 读写锁：读共享、写独占
    // 【mutable 的必要性】读方法（get/ttl/keys/size）都是 const 成员函数，
    //   但"读也要拿共享锁" —— mutable 允许 const 函数修改锁这个成员
    IndexMap idx_;
    // 【不变量】idx_ 的一切读写都发生在持锁区间（实现文件逐行保证）——
    //   这一条守住，Store 就是并发安全的；破了它，外面套多少原子都没用
};

}  // namespace mkv
