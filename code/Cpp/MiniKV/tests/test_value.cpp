// ============================================================================
// 【文件定位】test_value.cpp —— Value 类型查询与类型推断的冒烟测试
// ----------------------------------------------------------------------------
// 【测试思路】教程"每个 Step 写完就跑"的最小验证：直接构造已知值断言
//   类型查询正确；把各种形态的 token 喂给 valueFromToken 打印推断结果
//   —— 肉眼比对预期（教学测试不引入框架，assert + cout 就是最低成本）。
// ============================================================================

#include <iostream>
#include <cassert>        // assert：NDEBUG 未定义时生效（Debug 默认开）

#include "mkv/value.hpp"

using namespace mkv;

int main() {
    // test1: 类型查询
    Value v1 = std::int64_t{42};
    // 【显式 int64_t 构造的原因】直接写 42 会匹配 bool（历史上 variant 的
    //   重载决议坑）—— 显式 int64_t{42} 确保落到整数备选
    Value v2 = std::string("hello");
    assert(isInt(v1));        // 断言"42 是 int" —— 失败即 abort，测试红
    assert(isString(v2));
    std::cout << "v1 isInt? "    << isInt(v1)    << "\n";
    std::cout << "v2 isString? " << isString(v2) << "\n";

    // test2: 类型推断
    // 覆盖全部推断分支：整数/负整数/浮点/两个布尔/nil/普通串/混合串
    for (auto s : {"42", "-7", "3.14", "true", "false", "nil", "hello", "42abc"}) {
        std::cout << s << " -> " << formatValue(valueFromToken(s)) << "\n";
        // 预期："42"→(integer) 42；"3.14"→(double) 3.14；"42abc"→"42abc"（字符串兜底）
        // valueFromToken 收 const char* 隐式转 string_view —— 零拷贝直达
    }
    return 0;
}
