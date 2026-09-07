#include <iostream>

#include "mkv/store.hpp"
#include "mkv/value.hpp"

using namespace mkv;

int main() {
    Store s;
    s.set("name", std::string("zhangsan"));
    s.set("age",  std::int64_t{25});

    std::cout << "size = "      << s.size() << "\n";                                   // 2
    std::cout << "get name = "  << formatValue(s.get("name")->value) << "\n";          // "zhangsan"
    std::cout << "get nokey = " << (s.get("nokey") ? "hit" : "miss") << "\n";          // miss

    std::cout << "del age = "   << s.del("age") << "\n";                               // 1
    std::cout << "del age = "   << s.del("age") << "\n";                               // 0 幂等

    s.expire("name", 100);
    std::cout << "ttl name = "  << s.ttl("name")  << "\n";                             // 接近 100
    std::cout << "ttl nokey = " << s.ttl("nokey") << "\n";                             // -2

    // purgeExpired：过期 key 被后台清理
    s.set("temp", std::int64_t{1});
    s.expire("temp", 0);   // 立即过期
    s.purgeExpired();
    std::cout << "after purge, temp exists = " << (s.get("temp") ? "yes" : "no") << "\n";  // no
    return 0;
}
