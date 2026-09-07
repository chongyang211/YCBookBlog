#include <iostream>

#include "mkv/store.hpp"
#include "mkv/aof.hpp"
#include "mkv/value.hpp"

using namespace mkv;

int main() {
    const fs::path path = "data/test_aof.log";

    // 阶段 1：写两条 AOF 记录
    {
        AofWriter aof(path);
        aof.append("SET hello world\n");
        aof.append("SET name minikv\n");
        aof.flush();
    }

    // 阶段 2：重放还原状态
    Store store;
    auto r = replayAof(path, store);
    std::cout << "replayed " << r.replayed << " keys, corrupted " << r.corrupted << "\n";

    auto v = store.get("hello");
    if (v) std::cout << "hello = " << formatValue(v->value) << "\n";   // "world"

    // 阶段 3：再追加一行损坏数据，验证宽松模式容错
    {
        AofWriter aof(path);
        aof.append("SET\n");   // 缺少参数，坏行
        aof.flush();
    }
    Store store2;
    auto r2 = replayAof(path, store2);
    std::cout << "replayed " << r2.replayed << " keys, skipped "
              << r2.corrupted << " corrupted\n";
    return 0;
}
