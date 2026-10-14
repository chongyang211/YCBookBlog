// ============================================================================
// 【文件定位】test_store.cpp —— Store 七个核心方法的行为验证
// ----------------------------------------------------------------------------
// 【测试思路】按"写→读→删→过期→清理"的生命周期顺序逐方法走查：
//   每行注释标注预期值，跑完肉眼比对 —— 覆盖正常路径 + 边界
//   （删不存在的 key、TTL 三态、purge 生效）。
// ============================================================================

#include <iostream>

#include "mkv/store.hpp"
#include "mkv/value.hpp"

using namespace mkv;

int main() {
    Store s;      // 栈上仓库：不可拷贝（五法则 delete 过），只能引用传递
    s.set("name", std::string("zhangsan"));   // 显式 string：走 string 备选
    s.set("age",  std::int64_t{25});          // 显式 int64：防 bool 重载陷阱

    std::cout << "size = "      << s.size() << "\n";                                   // 2
    std::cout << "get name = "  << formatValue(s.get("name")->value) << "\n";          // "zhangsan"
    // get 返回 EntryPtr —— ->value 取字段再 formatValue 格式化
    std::cout << "get nokey = " << (s.get("nokey") ? "hit" : "miss") << "\n";          // miss
    // shared_ptr 的 bool 转换：判空即判"逻辑不存在"（含已过期）

    std::cout << "del age = "   << s.del("age") << "\n";                               // 1
    std::cout << "del age = "   << s.del("age") << "\n";                               // 0 幂等
    // 连删两次：第二次返回 0 —— erase 的返回值天然表达幂等语义

    s.expire("name", 100);
    std::cout << "ttl name = "  << s.ttl("name")  << "\n";                             // 接近 100
    // （100 或 99 都对 —— set 与 ttl 之间的毫秒差会吃掉不足一秒的部分）
    std::cout << "ttl nokey = " << s.ttl("nokey") << "\n";                             // -2
    // -2 哨兵：key 不存在（-1 永久 / >=0 剩余 的三态约定）

    // purgeExpired：过期 key 被后台清理
    s.set("temp", std::int64_t{1});
    s.expire("temp", 0);   // 立即过期
    // seconds=0 → expireAt = now —— 下一行 purge 时 now >= expireAt 成立即删
    s.purgeExpired();
    std::cout << "after purge, temp exists = " << (s.get("temp") ? "yes" : "no") << "\n";  // no
    // 主动清理验证：purge 后物理删除（不再是"读时视而不见"的惰性语义）
    return 0;
}
