#pragma once

#include <thread>          // std::jthread 是字段类型，要完整定义

namespace mkv {

class Store;               // 前置声明：构造参数是 Store&，前置声明足够

// 服务外壳：持有后台 TTL 清理线程（jthread 自动优雅停止）
class Server {
public:
    explicit Server(Store& store);

    // jthread 析构会自动 request_stop + join，我们不用写析构逻辑
    ~Server() = default;

    // 持有资源的类不允许拷贝
    Server(const Server&)            = delete;
    Server& operator=(const Server&) = delete;

private:
    Store&       store_;      // 引用成员：必须在构造函数初始化列表里绑定
    std::jthread purger_;     // C++20 协作式线程；析构会自动停 + join
};

}  // namespace mkv
