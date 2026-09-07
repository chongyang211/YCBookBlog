#include "mkv/server.hpp"

#include <chrono>
#include <stop_token>
#include <exception>

#include "mkv/store.hpp"    // 需要调 store_.purgeExpired()
#include "mkv/log.hpp"      // KV_LOG_INFO / KV_LOG_ERROR

namespace mkv {

// 构造函数：绑定 store_ 引用 + 启动 jthread
Server::Server(Store& store)
    : store_(store)
    , purger_(std::jthread([this](std::stop_token st) {
        // Lambda 是 jthread 的入口函数，参数 stop_token 由 jthread 自动注入
        KV_LOG_INFO("ttl purger thread started");
        while (!st.stop_requested()) {                              // 协作式停止
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            try {
                store_.purgeExpired();
            } catch (const std::exception& e) {
                // 捕获所有异常，绝不让后台线程因异常向上传播导致 std::terminate
                KV_LOG_ERROR("purger exception: " << e.what());
            }
        }
        KV_LOG_INFO("ttl purger thread stopped");
    }))
{}

}  // namespace mkv
