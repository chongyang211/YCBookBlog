// 实现文件必须先包含自己的头文件
#include "mkv/store.hpp"

#include <chrono>      // steady_clock / seconds / duration_cast
#include <memory>      // std::make_shared

namespace mkv {

// ============================================================================
// 【文件定位】store.cpp —— Store 八个方法的实现（并发安全的具体保障现场）
// ----------------------------------------------------------------------------
// 【实现通则】
//   ① 每个方法进来第一件事就是拿锁（RAII 守卫，作用域结束自动放）——
//     "锁保护路径完整覆盖共享数据的一切接触点"；
//   ② 读方法 shared_lock（可并发）、写方法 unique_lock（独占）——
//     锁类型与操作语义严格对应；
//   ③ 惰性删除渗透在读路径里：get/ttl/keys 遇到过期 key 一律当不存在。
// ============================================================================

// ─── 写方法 1：set ──────────────────────────────
// 存在则覆盖 + version++ + 清 TTL；不存在则新建 Entry
void Store::set(const std::string& key, Value v) {
    std::unique_lock lk(mu_);   // 写锁独占
    // CTAD（C++17）：模板参数从 mu_ 推导为 unique_lock<shared_mutex>；
    // 构造加锁、析构解锁 —— 提前 return/抛异常都不会漏解锁（RAII 锁守卫）

    auto it = idx_.find(key);
    if (it == idx_.end()) {
        // 新 key：堆上建 Entry，shared_ptr 管生命周期
        auto entry   = std::make_shared<Entry>();
        // 【为什么堆分配】Entry 放 map 的 value 是 EntryPtr —— 后台线程、
        //   GET 都可能持有同一 Entry，堆对象 + 引用计数 = 安全共享
        entry->value = std::move(v);                 // move 避免拷贝大 string
        // v 是按值传入的局部参数 —— move 进 Entry 后参数空壳，零多余拷贝
        idx_.emplace(key, std::move(entry));
        // emplace 原地构造 pair(key, EntryPtr)：比 insert 少一次临时 pair 移动
    } else {
        // 已有 key：直接改 Entry 内部字段（shared_ptr 指向的对象）
        it->second->value    = std::move(v);
        it->second->version += 1;
        // 【版本号自增】每次覆写 +1 —— 为将来 CAS 做铺垫：
        //   "version 没变 = 我读到的还作数"
        it->second->expireAt.reset();                // Redis 语义：SET 会清掉旧 TTL
        // 【Redis 语义还原】SET key val 覆盖后 key 变回永久 ——
        //   Redis 的 SET 默认不带 KEEPTTL，行为逐字对齐
    }
}

// ─── 读方法 2：get ──────────────────────────────
// 返回 nullptr 表示"逻辑上不存在"（含物理不存在 + 已过期两种情况）
EntryPtr Store::get(const std::string& key) const {
    std::shared_lock lk(mu_);   // 读锁可共享
    // 【共享锁的收益现场】多个线程同时 GET 不同 key —— 全部并发持读锁，
    //   互不阻塞（对比 unique_lock 的全串行，读吞吐 = 并发数倍）

    auto it = idx_.find(key);
    if (it == idx_.end()) return nullptr;
    if (it->second->isExpired()) return nullptr;     // 惰性删除：读到过期当不存在
    // 【惰性删除（TTL 双机制之一）】过期 key 不急着物理删除 ——
    //   读到时"视而不见"即可；物理清除交给后台 purger。
    //   好处：过期而无人问津的 key 不产生清理成本直到被扫到
    return it->second;                               // shared_ptr 拷贝，引用计数 +1
    // 【引用计数 +1 的意义】锁即将释放，后台线程随时可能 erase 掉这个
    //   map 条目 —— 但我们手里这份拷贝让它活着，用完才真正析构
}

// ─── 写方法 3：del ──────────────────────────────
std::size_t Store::del(const std::string& key) {
    std::unique_lock lk(mu_);
    return idx_.erase(key);   // unordered_map::erase(key) 返回删除的元素数（0 或 1）
    // 返回值天然就是"删没删着"：DEL 命令的 (integer) 1/0 直接由它驱动
    // 【不检查过期】即使 key 已过期但未清理，erase 也会删掉 ——
    //   对用户而言"过期=不存在"，删不存在的东西返回 0（合理）；
    //   这里删掉返回 1（语义边界：过期未清算的 key 仍占物理槽位）
}

// ─── 写方法 4：expire ───────────────────────────
bool Store::expire(const std::string& key, int seconds) {
    std::unique_lock lk(mu_);
    // 【为什么 expire 是写操作】改 expireAt = 改共享状态 ——
    //   读锁下改字段是数据竞争，必须独占

    auto it = idx_.find(key);
    if (it == idx_.end()) return false;
    // 用 steady_clock 而不是 system_clock：不受用户改系统时间影响
    it->second->expireAt = std::chrono::steady_clock::now()
                         + std::chrono::seconds(seconds);
    // 【绝对到期点】now + seconds 算出"何时到期"存下来 ——
    //   之后每次判断只做一次比较（比"存剩余秒数再倒扣"省事且不怕时钟回拨）
    return true;
}

// ─── 读方法 5：ttl ──────────────────────────────
// 约定：-2 = key 不存在；-1 = 存在但无 TTL（永久）；>=0 = 剩余秒数
std::int64_t Store::ttl(const std::string& key) const {
    std::shared_lock lk(mu_);

    auto it = idx_.find(key);
    if (it == idx_.end())      return -2;
    if (!it->second->expireAt) return -1;
    // 【三态哨兵】一个 int64 编码三种语义（Redis 同款协议）——
    //   客户端 switch(-2/-1/其他) 即可分辨，无需额外查询
    auto remaining = std::chrono::duration_cast<std::chrono::seconds>(
        *it->second->expireAt - std::chrono::steady_clock::now()).count();
    // 【时间差运算】time_point - time_point = duration ——
    //   duration_cast<seconds> 拿到秒数（向零截断）
    return remaining < 0 ? 0 : remaining;   // 已过期但没被清理的情况返回 0
    // 【边界打磨】过期与清理之间的窗口期返回 0 而非负数 ——
    //   "马上到期"比"负数剩余"对用户更友好（协议自洽）
}

// ─── 读方法 6：keys ─────────────────────────────
std::vector<std::string> Store::keys() const {
    std::shared_lock lk(mu_);

    std::vector<std::string> out;
    out.reserve(idx_.size());                        // 预分配避免多次扩容
    for (const auto& [k, v] : idx_) {                // C++17 结构化绑定
        // k 绑定 pair 的 key，v 绑定 EntryPtr —— 比它->first/it->second 可读
        if (!v->isExpired()) out.push_back(k);
        // 过滤过期 key：KEYS 的语义是"当前有效的键集"（惰性删除渗透）
    }
    return out;
    // 返回值拷贝（NRVO 优化下零拷贝）：锁内构建快照、锁外使用 ——
    // 调用方拿到的 vector 与 store 内部状态再无关联（不持锁引用）
}

// ─── 读方法 7：size ─────────────────────────────
std::size_t Store::size() const noexcept {
    std::shared_lock lk(mu_);
    return idx_.size();
    // 【语义说明】size 含"过期未清理"的 key —— 物理计数；
    //   KEYS 的过滤语义与之不同，教学版保留这个差异（Redis 的 DBSIZE 同款）
}

// ─── 维护方法：purgeExpired ─────────────────────
// 由后台 jthread 定期调用；这里用"erase 返回下一个迭代器"的正确写法
void Store::purgeExpired() {
    std::unique_lock lk(mu_);
    // 【锁语义考量】清理是删除操作（改哈希结构）→ 独占写锁；
    //   持锁期间 GET 们全部阻塞 —— 所以清理必须"快进快出"
    //   （100ms 一次的频率就是权衡：太频繁占锁，太稀疏过期 key 堆积）

    for (auto it = idx_.begin(); it != idx_.end(); ) {
        if (it->second->isExpired()) {
            it = idx_.erase(it);        // erase 返回下一个有效迭代器
            // 【教程§06"迭代器失效"现场的核心知识点】
            //   错误写法：idx_.erase(it); ++it;  ← erase 后 it 已失效，
            //   再 ++it 是 UB（崩溃或诡异行为）—— 教程 Step 5.3 的造 BUG 高峰；
            //   正确写法：接收 erase 的返回值（下一个迭代器），不再自增
        } else {
            ++it;
        }
    }
}

}  // namespace mkv
