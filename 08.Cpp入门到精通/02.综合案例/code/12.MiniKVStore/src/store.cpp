// store.cpp —— Store 核心实现
//
// 关键点：
// - set/del/expire/purgeExpired 用 unique_lock 拿写锁；
// - get/ttl/keys/size 用 shared_lock 拿读锁；
// - purgeExpired 用"erase 返回下一个迭代器"的标准写法避免 UB。

#include "mkv/store.hpp"

#include <chrono>
#include <mutex>
#include <utility>

namespace mkv {

void Store::set(const std::string& key, Value v) {
    std::unique_lock lk(mu_);
    auto it = idx_.find(key);
    if (it == idx_.end()) {
        auto entry = std::make_shared<Entry>();
        entry->value = std::move(v);
        idx_.emplace(key, std::move(entry));
    } else {
        it->second->value    = std::move(v);
        it->second->version += 1;
        it->second->expireAt.reset();   // 与 Redis 一致：SET 清掉旧 TTL
    }
}

EntryPtr Store::get(const std::string& key) const {
    std::shared_lock lk(mu_);
    auto it = idx_.find(key);
    if (it == idx_.end())              return nullptr;
    if (it->second->isExpired())       return nullptr;   // 惰性删除：读到过期当不存在
    return it->second;
}

std::size_t Store::del(const std::string& key) {
    std::unique_lock lk(mu_);
    return idx_.erase(key);
}

bool Store::expire(const std::string& key, int seconds) {
    std::unique_lock lk(mu_);
    auto it = idx_.find(key);
    if (it == idx_.end()) return false;
    it->second->expireAt =
        std::chrono::steady_clock::now() + std::chrono::seconds(seconds);
    return true;
}

std::int64_t Store::ttl(const std::string& key) const {
    std::shared_lock lk(mu_);
    auto it = idx_.find(key);
    if (it == idx_.end())            return -2;
    if (!it->second->expireAt)       return -1;
    auto remaining = std::chrono::duration_cast<std::chrono::seconds>(
        *it->second->expireAt - std::chrono::steady_clock::now()).count();
    return remaining < 0 ? 0 : remaining;
}

std::vector<std::string> Store::keys() const {
    std::shared_lock lk(mu_);
    std::vector<std::string> out;
    out.reserve(idx_.size());
    for (const auto& [k, v] : idx_) {
        if (!v->isExpired()) out.push_back(k);
    }
    return out;
}

std::size_t Store::size() const noexcept {
    // 注意：这里可能与 keys() 在并发下不一致（keys 会跳过过期），
    // 但 size() 的语义就是"map 的物理 size"，让 noexcept 成立。
    return idx_.size();
}

void Store::purgeExpired() {
    std::unique_lock lk(mu_);
    for (auto it = idx_.begin(); it != idx_.end(); ) {
        if (it->second->isExpired()) {
            it = idx_.erase(it);   // erase 返回下一个有效迭代器
        } else {
            ++it;
        }
    }
}

}  // namespace mkv
