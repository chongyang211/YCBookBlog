
#pragma once
#include <chrono>
#include <optional>
#include <string>

#include "value.hpp"

namespace mkv {
    struct Entry {
        Value value;
        // 绝对过期时间点；nullopt 表示永不过期
        std::optional<std::chrono::steady_clock::time_point> expireAt;
        std::chrono::steady_clock::time_point createdAt = std::chrono::steady_clock::now();
        // 每次被 SET 覆写就 +1，用于将来做 CAS
        std::uint64_t version = 0;
        [[nodiscard]] bool isExpired() const noexcept {
            return expireAt.has_value() && std::chrono::steady_clock::now() >= *expireAt;
        }
    };

    
}

