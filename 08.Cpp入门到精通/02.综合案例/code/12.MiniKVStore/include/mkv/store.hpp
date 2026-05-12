// mkv/store.hpp —— KV 数据中心
//
// 关键设计（详见 12.迷你KV存储引擎.md §06、§10）：
// - 五大特殊成员一次性 = delete：Store 是单例式中心组件，禁止拷贝 / 移动；
// - 用 std::shared_mutex（C++17）支持读多写少；
// - mutable mu_：const 方法（get/keys/ttl/size）也能上锁；
// - 失败用返回值表达（nullptr / -2 / -1），异常只用于"调用方写错了"。

#pragma once

#include "mkv/entry.hpp"
#include "mkv/value.hpp"

#include <shared_mutex>
#include <string>
#include <vector>
#include <cstddef>
#include <cstdint>

namespace mkv {

class Store {
public:
    Store() = default;

    // 五法则：禁止拷贝 / 移动
    Store(const Store&)            = delete;
    Store& operator=(const Store&) = delete;
    Store(Store&&)                 = delete;
    Store& operator=(Store&&)      = delete;

    // ====== 命令对应的底层操作 ======
    void          set     (const std::string& key, Value v);
    EntryPtr      get     (const std::string& key) const;          // 不存在 / 已过期返回 nullptr
    std::size_t   del     (const std::string& key);                // 删了返回 1，否则 0
    bool          expire  (const std::string& key, int seconds);   // 设过期；key 不存在返回 false
    std::int64_t  ttl     (const std::string& key) const;          // 剩余秒；-2 不存在 -1 永久
    std::vector<std::string> keys() const;
    std::size_t   size() const noexcept;

    // ====== 维护操作 ======
    void          purgeExpired();   // 后台 TTL 线程调用：扫描并删除过期 key

private:
    mutable std::shared_mutex mu_;
    IndexMap                  idx_;
};

}  // namespace mkv
