#pragma once        // 头文件保护（编译器内置指令）

#include <queue>                // std::queue：底层 FIFO 容器
#include <mutex>                // std::mutex / lock_guard / unique_lock
#include <condition_variable>   // std::condition_variable：条件变量
#include <optional>             // std::optional：表达"pop 可能没有结果"

// ============================================================================
// 【组件定位】BlockingQueue —— 线程安全的阻塞队列：生产者-消费者模式的灵魂
// ----------------------------------------------------------------------------
// 【设计思路·教程阶段①的演进史】
//   Step 1.1 裸版（不加锁）：单线程 FIFO 跑通，看起来完美；
//   Step 1.2 双线程作死：亲眼看 race condition（丢数据/崩溃/卡死）；
//   Step 1.3 加 mutex：数据竞争修复，但消费者只能"忙等"轮询（浪费 CPU）;
//   Step 1.4 加 condition_variable：空队列时挂起线程（不忙等）+ shutdown 优雅停机。
// 【三个关键决策】
//   ① 锁保护的是"队列结构"（q + stopped 两个共享状态）；
//   ② 谓词版 wait 防虚假唤醒（spurious wakeup）；
//   ③ shutdown 用"停止标志 + notify_all"让所有阻塞的 pop 优雅返回 nullopt。
// ============================================================================

// 线程安全的阻塞队列：生产者-消费者模式的灵魂
// - push 时通知一个等待的消费者
// - pop 在队列空时阻塞（不忙等），收到 shutdown 信号后返回 nullopt
template <typename T>
class BlockingQueue {
private:
    std::queue<T>             q;
    // 【为什么必须锁】queue 的 push/pop 都要改内部指针和 size —— 两个线程
    // 同时 push 会破坏链表结构（经典 race condition，教程 Step 1.2 现场）。
    mutable std::mutex        mtx;
    // mutable 的必要性：size() 是 const 成员函数，但"读 size"也要拿锁
    // （读期间别人在 push/pop 就是数据竞争）—— mutable 允许 const 函数改锁。
    std::condition_variable   cvNotEmpty;     // 队列非空时通知
    // 【机制】消费者在 cv 上"睡眠等通知"；生产者 push 后 notify_one 踢醒一个。
    // 消费者醒来后重新检查谓词（队列是否真的非空）—— 谓词必须由锁保护。
    bool                      stopped = false;
    // 【停机语义】shutdown 后：push 直接丢弃（返回），pop 若空则返回 nullopt ——
    // 工人线程拿到 nullopt 就知道自己该下班了（线程池优雅停机的基石）。

public:
    // 生产者接口：入队 + 通知
    void push(T item) {
        {   // 花括号限定锁的作用域：通知动作本身不需要持锁
            std::lock_guard<std::mutex> lock(mtx);
            if (stopped) return;              // 已停机：丢弃新任务（不再叫醒任何人）
            q.push(std::move(item));          // move 入队：模板参数 T 的所有权交队列，零拷贝
        }
        cvNotEmpty.notify_one();              // 锁外通知（性能更好）
        // 【锁外通知的原理】notify 只是"踢醒睡眠者"，唤醒后对方还要抢锁、
        // 检查谓词 —— 通知本身不碰共享数据，持锁通知只会让被唤醒者
        // "醒来发现锁还被通知者攥着，白醒一次再睡"（hurry up and wait）。
    }

    // 阻塞 pop：队列空时挂起线程，不再轮询
    std::optional<T> pop() {
        std::unique_lock<std::mutex> lock(mtx);            // 必须用 unique_lock
        // 【对比 lock_guard】wait 内部要"解锁→睡眠→被唤醒→重新加锁"，
        // lock_guard 没有 lock/unlock 接口，只有 unique_lock 能配合 cv。
        cvNotEmpty.wait(lock, [this]{                      // 谓词防虚假唤醒
            return !q.empty() || stopped;
        });
        // 【谓词版 wait 等价于】
        //   while (!(!q.empty() || stopped)) cv.wait(lock);
        // 【虚假唤醒】OS 层面的 wait 可能"无人通知也醒来"（历史遗留设计），
        // 裸 wait 醒来就往下走会取空队列 —— 谓词版"醒了先验货"，
        // 条件不满足继续睡，杜绝伪醒。
        // 【两个出口条件】队列非空（有活干）或已停机（该下班）。
        if (q.empty() && stopped) return std::nullopt;     // 收到关闭信号
        // 走到这里队列必然非空（谓词保证）—— 安全取队头

        T item = std::move(q.front());     // move 拿走元素所有权（出队零拷贝）
        q.pop();                           // 弹掉空壳（front 已被掏空）
        return item;                       // optional 装上值（隐式构造）
    }

    // 关闭：通知所有 pop 中的线程退出
    void shutdown() {
        {
            std::lock_guard<std::mutex> lock(mtx);
            stopped = true;                // 停机标志也是共享状态，必须锁内改
        }
        cvNotEmpty.notify_all();           // 唤醒所有等待者
        // 【为什么是 notify_all】工人可能有 N 个都阻塞在 pop 睡眠中，
        // notify_one 一次只能叫醒一个 —— 其余的永远睡死（线程池 join 卡死）。
        // all 把所有睡眠者叫醒，各自检查谓词后集体拿到 nullopt 下班。
        // 【语义细节】已入队的任务仍可被取走（pop 先检查队列再检查 stopped）——
        // "排干存量、拒绝增量"的温和停机。
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(mtx);
        // 连"读 size"也要锁：另一线程正在 push 时读 size 就是数据竞争
        return q.size();
    }
};
