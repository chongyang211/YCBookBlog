
#include <iostream>

#include "mkv/value.hpp"

namespace mkv {

    void test1() {
        mkv::Value v1 = std::int64_t{42};
        mkv::Value v2 = std::string("hello");
        std::cout << "v1 isInt? "    << mkv::isInt(v1)    << "\n";
        std::cout << "v2 isString? " << mkv::isString(v2) << "\n";
    }

    void test2() {
        for (auto s : {"42", "-7", "3.14", "true", "false", "nil", "hello", "42abc"}) {
            std::cout << s << " -> " << mkv::formatValue(mkv::valueFromToken(s)) << "\n";
        }

    }

}
