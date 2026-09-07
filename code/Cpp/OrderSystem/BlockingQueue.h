#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>

// 线程安全的阻塞队列：生产者-消费者模式的灵魂
// - push 时通知一个等待的消费者
// - pop 在队列空时阻塞（不忙等），收到 shutdown 信号后返回 nullopt
template <typename T>
class BlockingQueue {
private:
    std::queue<T>             q;
    mutable std::mutex        mtx;
    std::condition_variable   cvNotEmpty;     // 队列非空时通知
    bool                      stopped = false;

public:
    void push(T item) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            if (stopped) return;
            q.push(std::move(item));
        }
        cvNotEmpty.notify_one();              // 锁外通知（性能更好）
    }

    // 阻塞 pop：队列空时挂起线程，不再轮询
    std::optional<T> pop() {
        std::unique_lock<std::mutex> lock(mtx);            // 必须用 unique_lock
        cvNotEmpty.wait(lock, [this]{                      // 谓词防虚假唤醒
            return !q.empty() || stopped;
        });
        if (q.empty() && stopped) return std::nullopt;     // 收到关闭信号

        T item = std::move(q.front());
        q.pop();
        return item;
    }

    // 关闭：通知所有 pop 中的线程退出
    void shutdown() {
        {
            std::lock_guard<std::mutex> lock(mtx);
            stopped = true;
        }
        cvNotEmpty.notify_all();              // 唤醒所有等待者
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(mtx);
        return q.size();
    }
};
