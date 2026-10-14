#include "mkv/server.hpp"

#include <chrono>        // sleep_for / milliseconds
#include <stop_token>    // std::stop_token（jthread 协作停止的凭证）
#include <exception>     // std::exception

#include "mkv/store.hpp"    // 需要调 store_.purgeExpired()
#include "mkv/log.hpp"      // KV_LOG_INFO / KV_LOG_ERROR

namespace mkv {

// 构造函数：绑定 store_ 引用 + 启动 jthread
// 【成员初始化顺序】store_（引用绑定）→ purger_（线程启动）——
//   顺序由声明序决定：先绑好 store_ 再起线程，lambda 捕 this 访问的
//   store_ 必然已就绪（构造竞态的防御性排序）
Server::Server(Store& store)
    : store_(store)
    , purger_(std::jthread([this](std::stop_token st) {
        // Lambda 是 jthread 的入口函数，参数 stop_token 由 jthread 自动注入
        // 【jthread 的签名约定】可调用对象的"第一个参数"若为 stop_token，
        //   jthread 会自动把自己内部的 stop_state 传进来 ——
        //   析构时 request_stop() 置位的就是它（协作式停止的信号线）
        KV_LOG_INFO("ttl purger thread started");
        while (!st.stop_requested()) {                              // 协作式停止
            // 【协作式 vs 杀死式】不调 pthread_cancel 强杀（任意点断电式
            //   终止会破坏锁/内存不变量）—— 循环每轮主动查"该停了吗"，
            //   停在安全点（睡醒后、持锁前）—— 优雅退出的标准姿势
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            // 【100ms 的权衡】睡在循环顶部：停止请求最多 100ms 内响应；
            //   频率太高 = 空转占 CPU，太低 = 过期 key 迟迟不清（内存滞留）
            try {
                store_.purgeExpired();       // 拿写锁扫一遍并删除过期 key
            } catch (const std::exception& e) {
                // 捕获所有异常，绝不让后台线程因异常向上传播导致 std::terminate
                // 【后台线程的铁律】线程函数逃出的异常没人接（栈展开到
                //   线程边界）→ std::terminate 全进程崩溃 —— 主线程的
                //   try-catch 管不到别的线程，这里必须自己兜住
                KV_LOG_ERROR("purger exception: " << e.what());
            }
        }
        KV_LOG_INFO("ttl purger thread stopped");
        // 优雅退出日志：与启动日志成对（线程生命周期可观测）
    }))
{}

}  // namespace mkv
