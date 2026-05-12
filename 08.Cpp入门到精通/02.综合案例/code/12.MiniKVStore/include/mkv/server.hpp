// mkv/server.hpp —— 后台 TTL 清理服务（jthread + stop_token）
//
// 关键设计（详见 12.迷你KV存储引擎.md §11）：
// - std::jthread（C++20）：析构自动 request_stop + join，告别 std::thread 忘 join 即 terminate；
// - 通过 stop_token 协作式停止，循环里检查 stop_requested()；
// - 析构顺序：server 必须先于 store 析构（栈上声明顺序保证）。

#pragma once

#include <thread>

namespace mkv {

class Store;

class Server {
public:
    explicit Server(Store& store);

    // 析构时 jthread 自动 request_stop + join
    ~Server() = default;

    Server(const Server&)            = delete;
    Server& operator=(const Server&) = delete;
    Server(Server&&)                 = delete;
    Server& operator=(Server&&)      = delete;

private:
    Store&       store_;
    std::jthread purger_;
};

}  // namespace mkv
