#pragma once

#include <shared_mutex>
#include <string>
#include <vector>
#include <cstdint>

#include "entry.hpp"

namespace mkv {

class Store {
public:
    Store() = default;

    // 五法则：Store 是有所有权的资源管理者，禁止拷贝
    Store(const Store&)            = delete;
    Store& operator=(const Store&) = delete;
    Store(Store&&)                 = delete;
    Store& operator=(Store&&)      = delete;

    // ====== 命令对应的底层操作 ======
    void          set     (const std::string& key, Value v);
    EntryPtr      get     (const std::string& key) const;          // 不存在返回 nullptr
    std::size_t   del     (const std::string& key);                // 删了返回 1，否则 0
    bool          expire  (const std::string& key, int seconds);   // 设过期；key 不存在返回 false
    std::int64_t  ttl     (const std::string& key) const;          // 剩余秒；-2 不存在 -1 永久
    std::vector<std::string> keys() const;
    std::size_t   size() const noexcept;

    // ====== 维护操作 ======
    void          purgeExpired();   // 后台线程调用：扫描并删除过期 key

private:
    mutable std::shared_mutex mu_;   // 读写锁：读共享、写独占
    IndexMap idx_;
};

}  // namespace mkv
