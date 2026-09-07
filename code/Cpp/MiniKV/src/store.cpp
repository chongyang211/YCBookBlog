// 实现文件必须先包含自己的头文件
#include "mkv/store.hpp"

#include <chrono>
#include <memory>

namespace mkv {

// ─── 写方法 1：set ──────────────────────────────
// 存在则覆盖 + version++ + 清 TTL；不存在则新建 Entry
void Store::set(const std::string& key, Value v) {
    std::unique_lock lk(mu_);   // 写锁独占

    auto it = idx_.find(key);
    if (it == idx_.end()) {
        // 新 key：堆上建 Entry，shared_ptr 管生命周期
        auto entry   = std::make_shared<Entry>();
        entry->value = std::move(v);                 // move 避免拷贝大 string
        idx_.emplace(key, std::move(entry));
    } else {
        // 已有 key：直接改 Entry 内部字段（shared_ptr 指向的对象）
        it->second->value    = std::move(v);
        it->second->version += 1;
        it->second->expireAt.reset();                // Redis 语义：SET 会清掉旧 TTL
    }
}

// ─── 读方法 2：get ──────────────────────────────
// 返回 nullptr 表示"逻辑上不存在"（含物理不存在 + 已过期两种情况）
EntryPtr Store::get(const std::string& key) const {
    std::shared_lock lk(mu_);   // 读锁可共享

    auto it = idx_.find(key);
    if (it == idx_.end()) return nullptr;
    if (it->second->isExpired()) return nullptr;     // 惰性删除：读到过期当不存在
    return it->second;                               // shared_ptr 拷贝，引用计数 +1
}

// ─── 写方法 3：del ──────────────────────────────
std::size_t Store::del(const std::string& key) {
    std::unique_lock lk(mu_);
    return idx_.erase(key);   // unordered_map::erase(key) 返回删除的元素数（0 或 1）
}

// ─── 写方法 4：expire ───────────────────────────
bool Store::expire(const std::string& key, int seconds) {
    std::unique_lock lk(mu_);

    auto it = idx_.find(key);
    if (it == idx_.end()) return false;
    // 用 steady_clock 而不是 system_clock：不受用户改系统时间影响
    it->second->expireAt = std::chrono::steady_clock::now()
                         + std::chrono::seconds(seconds);
    return true;
}

// ─── 读方法 5：ttl ──────────────────────────────
// 约定：-2 = key 不存在；-1 = 存在但无 TTL（永久）；>=0 = 剩余秒数
std::int64_t Store::ttl(const std::string& key) const {
    std::shared_lock lk(mu_);

    auto it = idx_.find(key);
    if (it == idx_.end())      return -2;
    if (!it->second->expireAt) return -1;
    auto remaining = std::chrono::duration_cast<std::chrono::seconds>(
        *it->second->expireAt - std::chrono::steady_clock::now()).count();
    return remaining < 0 ? 0 : remaining;   // 已过期但没被清理的情况返回 0
}

// ─── 读方法 6：keys ─────────────────────────────
std::vector<std::string> Store::keys() const {
    std::shared_lock lk(mu_);

    std::vector<std::string> out;
    out.reserve(idx_.size());                        // 预分配避免多次扩容
    for (const auto& [k, v] : idx_) {                // C++17 结构化绑定
        if (!v->isExpired()) out.push_back(k);
    }
    return out;
}

// ─── 读方法 7：size ─────────────────────────────
std::size_t Store::size() const noexcept {
    std::shared_lock lk(mu_);
    return idx_.size();
}

// ─── 维护方法：purgeExpired ─────────────────────
// 由后台 jthread 定期调用；这里用"erase 返回下一个迭代器"的正确写法
void Store::purgeExpired() {
    std::unique_lock lk(mu_);

    for (auto it = idx_.begin(); it != idx_.end(); ) {
        if (it->second->isExpired()) {
            it = idx_.erase(it);        // erase 返回下一个有效迭代器
        } else {
            ++it;
        }
    }
}

}  // namespace mkv
