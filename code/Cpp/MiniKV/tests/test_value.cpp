#include <iostream>
#include <cassert>

#include "mkv/value.hpp"

using namespace mkv;

int main() {
    // test1: 类型查询
    Value v1 = std::int64_t{42};
    Value v2 = std::string("hello");
    assert(isInt(v1));
    assert(isString(v2));
    std::cout << "v1 isInt? "    << isInt(v1)    << "\n";
    std::cout << "v2 isString? " << isString(v2) << "\n";

    // test2: 类型推断
    for (auto s : {"42", "-7", "3.14", "true", "false", "nil", "hello", "42abc"}) {
        std::cout << s << " -> " << formatValue(valueFromToken(s)) << "\n";
    }
    return 0;
}
