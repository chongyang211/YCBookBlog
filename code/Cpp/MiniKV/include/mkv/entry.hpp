#pragma once

#include <chrono>            // steady_clock / time_point / seconds
#include <optional>          // std::optional：表达"可能没有过期时间"
#include <string>            // std::string（key 类型）
#include <cstdint>           // std::uint64_t
#include <unordered_map>     // std::unordered_map：哈希索引
#include <memory>            // std::shared_ptr / make_shared

#include "value.hpp"         // Value 类型（Entry 的字段）

namespace mkv {

// ============================================================================
// 【模块定位】entry.hpp —— Entry：单条键值对（值 + TTL + 版本）与索引类型
// ----------------------------------------------------------------------------
// 【设计思想·教程§04】
//   ① "值 + 元数据"打包：TTL/创建时间/版本是"每个 key 的附属状态"，
//     与 value 同生共死 —— 打包成一个 Entry 而非三张平行表（避免对齐复杂度）；
//   ② expireAt 用 optional<nullopt 表永久>：比"0 表永久"的魔数约定
//     类型安全（nullopt 不可能与任何时间点混淆）；
//   ③ steady_clock：TTL 的唯一正确时钟（system_clock 被用户改时间
//     会让所有 TTL 瞬间失效或永不过期 —— 教程"关键设计取舍"明文强调）。
// ============================================================================

struct Entry {
    Value value;
    // 【variant 直接做成员】Entry 大小 ≈ 最大备选（string）+ 标签，
    //   值语义天然可拷贝 —— 配合下面的 shared_ptr 语义刚刚好。

    // 绝对过期时间点；nullopt 表示永不过期
    std::optional<std::chrono::steady_clock::time_point> expireAt;
    // 【绝对时间点而非剩余秒数】存"到期那一刻"每次判断只需一次比较；
    //   存剩余秒数每次都要减再比且会被中途暂停干扰 —— Redis 同款设计。

    std::chrono::steady_clock::time_point createdAt = std::chrono::steady_clock::now();
    // NSDMI 类内默认初始化：不传就取"出生时刻"（创建时间属于审计信息，
    //   当前版本只存不用 —— 为将来 LRU 淘汰等策略预留）

    // 每次被 SET 覆写就 +1，用于将来做 CAS
    std::uint64_t version = 0;
    // 【乐观并发的伏笔】Store::set 在"覆盖已有 key"时 version++ ——
    //   将来做 CAS（compare-and-swap）时用它判断"我读到的值没被别人改过"。

    [[nodiscard]] bool isExpired() const noexcept {
        // [[nodiscard]]：调用方忽略返回值大概率是 bug（判断过期不看结果没意义），
        //   编译器给警告 —— 现代 C++ 接口设计的标配属性
        return expireAt.has_value() && std::chrono::steady_clock::now() >= *expireAt;
        // 双重判定：① 没设过期（nullopt）直接 false（has_value 短路）；
        //           ② 到期点已过 → true。* 解引用 optional 取时间点比较。
    }
};

using EntryPtr = std::shared_ptr<Entry>;
// 【为什么 shared_ptr 而非 unique_ptr（README"关键设计取舍"第一条）】
//   后台清理线程扫描、GET 借用、AOF 写盘可能"同时拿着同一个 Entry" ——
//   引用计数保证"正在被使用的对象不会被提前析构"：
//   清理线程 erase 掉 map 里的条目时，GET 正持有的那份拷贝仍活着，
//   用完自然释放 —— unique_ptr 的独占语义做不到多方共用。

using IndexMap = std::unordered_map<std::string, EntryPtr>;
// 【为什么 unordered_map 而非 map】KV 语义只需 O(1) 点查，
//   不需要有序遍历（KEYS 输出顺序无承诺）—— 哈希表是性能正解。

}  // namespace mkv
