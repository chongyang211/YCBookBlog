// mkv/entry.hpp —— KV 条目的实体类型
//
// 关键设计（详见 12.迷你KV存储引擎.md §04）：
// - Entry = Value + 过期时间 + 元数据；
// - 用 std::optional<time_point> 表"可选过期"，避免魔法值；
// - 用 steady_clock 而不是 system_clock，防止用户改系统时间打乱 TTL；
// - 用 shared_ptr<Entry> 让"后台清理线程"和"前台 GET"安全共享同一条记录。

#pragma once

#include "mkv/value.hpp"

#include <chrono>
#include <optional>
#include <unordered_map>
#include <memory>
#include <string>
#include <cstdint>

namespace mkv {

struct Entry {
    Value value;

    // 绝对过期时间点；nullopt 表示永不过期
    std::optional<std::chrono::steady_clock::time_point> expireAt;

    std::chrono::steady_clock::time_point createdAt =
        std::chrono::steady_clock::now();

    // 每次被 SET 覆写就 +1，预留给将来做 CAS / 乐观锁
    std::uint64_t version = 0;

    [[nodiscard]] bool isExpired() const noexcept {
        return expireAt.has_value() &&
               std::chrono::steady_clock::now() >= *expireAt;
    }
};

using EntryPtr = std::shared_ptr<Entry>;
using IndexMap = std::unordered_map<std::string, EntryPtr>;

}  // namespace mkv
