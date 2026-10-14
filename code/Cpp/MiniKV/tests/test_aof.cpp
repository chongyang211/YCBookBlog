// ============================================================================
// 【文件定位】test_aof.cpp —— AOF 写盘 + 重放 + 损坏容错的三阶段验证
// ----------------------------------------------------------------------------
// 【测试思路】对应教程§08 的三个 Step：
//   阶段 1：写两条记录（花括号限定 AofWriter 作用域 —— 出花括号即析构关文件）；
//   阶段 2：新 Store 重放，验证状态还原；
//   阶段 3：追加一条"坏行"（缺参数的 SET），验证宽松模式跳过它继续工作
//   —— 教程"手改 aof.log 制造截断"造 BUG 环节的自动化版本。
// ============================================================================

#include <iostream>

#include "mkv/store.hpp"
#include "mkv/aof.hpp"
#include "mkv/value.hpp"

using namespace mkv;

int main() {
    const fs::path path = "data/test_aof.log";
    // 相对路径：需从项目根目录运行（data/ 由 AofWriter 构造时自动创建）

    // 阶段 1：写两条 AOF 记录
    {
        // 花括号作用域 = RAII 的时间边界：出块即析构 → flush 落盘 → 关文件
        AofWriter aof(path);
        aof.append("SET hello world\n");     // 手工拼行（绕过命令对象，
        aof.append("SET name minikv\n");     //   直测 AofWriter 本体）
        aof.flush();
    }

    // 阶段 2：重放还原状态
    Store store;
    auto r = replayAof(path, store);
    // 默认 tolerant=true：宽松模式（教学版推荐配置）
    std::cout << "replayed " << r.replayed << " keys, corrupted " << r.corrupted << "\n";
    // 预期 replayed=2, corrupted=0

    auto v = store.get("hello");
    if (v) std::cout << "hello = " << formatValue(v->value) << "\n";   // "world"
    // get→判空→formatValue：与 test_store 相同的验证三连

    // 阶段 3：再追加一行损坏数据，验证宽松模式容错
    {
        AofWriter aof(path);
        // app 模式：追加不覆盖 —— 前面两条记录原封不动
        aof.append("SET\n");   // 缺少参数，坏行
        // makeCommand 里 needArgs(2,"SET") 会抛 CmdSyntaxError —— 坏行样本
        aof.flush();
    }
    Store store2;
    auto r2 = replayAof(path, store2);
    std::cout << "replayed " << r2.replayed << " keys, skipped "
              << r2.corrupted << " corrupted\n";
    // 预期 replayed=2（好行照常重放）, skipped=1（坏行被宽松模式跳过并告警）
    return 0;
}
