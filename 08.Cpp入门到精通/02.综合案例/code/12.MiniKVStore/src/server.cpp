// server.cpp —— 后台 TTL 清理服务实现

#include "mkv/server.hpp"
#include "mkv/store.hpp"
#include "mkv/log.hpp"

#include <chrono>
#include <exception>
#include <stop_token>
#include <thread>

namespace mkv {

Server::Server(Store& store) : store_(store) {
    purger_ = std::jthread([this](std::stop_token st) {
        KV_LOG_INFO("ttl purger thread started");
        while (!st.stop_requested()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            try {
                store_.purgeExpired();
            } catch (const std::exception& e) {
                KV_LOG_ERROR("purger exception: " << e.what());
            }
        }
        KV_LOG_INFO("ttl purger thread stopped");
    });
}

}  // namespace mkv
