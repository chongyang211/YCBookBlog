#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace mkv {

    enum class CmdType {
        Set, Get, Del, Expire, Ttl, Keys, Save, Exit, Unknown
    };

    CmdType parseCmdType(const std::string& s);

    std::vector<std::string_view> tokenize(std::string_view line);

}

