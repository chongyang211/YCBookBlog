#pragma once

#include <thread>          // std::jthread 是字段类型，要完整定义

namespace mkv {

// ============================================================================
// 【模块定位】server.hpp —— Server：服务外壳（后台 TTL 清理线程的宿主）
// ----------------------------------------------------------------------------
// 【设计思想】TTL 双机制之一：Store::get 的"惰性删除"（读到过期当不存在）
//   只清理"被访问的 key"—— 冷 key 永远占内存；后台 purger 每 100ms 全量
//   扫描补上"主动清理"（教程"TTL 过期：惰性 + 主动双机制"）。
// 【jthread vs thread（C++20 新特性落地）】
//   thread：忘 join → 析构 terminate（教学项目 OrderSystem 手写 shutdown 兜底）；
//   jthread：析构自动 request_stop + join —— RAII 线程，一行析构都不用写。
// ============================================================================

class Store;               // 前置声明：构造参数是 Store&，前置声明足够
// 成员 store_ 是引用（指针大小）—— 类型完整定义非必需，.cpp 里再 include

// 服务外壳：持有后台 TTL 清理线程（jthread 自动优雅停止）
class Server {
public:
    explicit Server(Store& store);
    // explicit：单参构造禁隐式转换

    // jthread 析构会自动 request_stop + join，我们不用写析构逻辑
    ~Server() = default;
    // 【对比 OrderSystem 的 ThreadPool】那里析构要手动 shutdown + 循环
    //   join（C++17 thread）；这里 = default 即可 —— 语言进化的直接受益

    // 持有资源的类不允许拷贝
    // 引用成员 + jthread（不可拷贝）本身就禁了拷贝 —— 显式 = delete 是
    // "把隐式规则写成文档"的防御性声明
    Server(const Server&)            = delete;
    Server& operator=(const Server&) = delete;

private:
    Store&       store_;      // 引用成员：必须在构造函数初始化列表里绑定
    // 【引用成员的规矩】构造后不可重绑、类因此天然不可赋值 ——
    //   Server 的生命周期承诺"服务同一个 Store"
    std::jthread purger_;     // C++20 协作式线程；析构会自动停 + join
    // jthread 构造时 lambda 的第一个参数自动注入 stop_token ——
    //   线程函数凭它感知"该停了"（协作式而非杀死式，随时可在安全点退出）
};

}  // namespace mkv
