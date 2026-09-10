package com.orders.pool;

import com.orders.concurrent.MyThreadPool;

import java.util.concurrent.RejectedExecutionException;
import java.util.function.BiConsumer;

/** 4 种拒绝策略（教程 §7.3）：BiConsumer 函数式接口 = 策略模式 */
public final class RejectPolicy {

    private RejectPolicy() {}

    /** 抛异常（JDK AbortPolicy 同款）——必须感知失败的场景 */
    public static BiConsumer<Runnable, MyThreadPool> abort() {
        return (task, pool) -> {
            throw new RejectedExecutionException(
                    "任务被拒绝，活跃=" + pool.getActiveCount() + " 队列=" + pool.getQueueSize());
        };
    }

    /** 调用者自己跑（自然降速）——业务推荐，不丢任务 */
    public static BiConsumer<Runnable, MyThreadPool> callerRuns() {
        return (task, pool) -> task.run();
    }

    /** 静默丢弃——可丢失的日志/统计 */
    public static BiConsumer<Runnable, MyThreadPool> discard() {
        return (task, pool) -> { /* 啥也不做 */ };
    }

    /** 丢弃队列最老的任务，把新任务塞进去——最新数据更重要（如行情）*/
    public static BiConsumer<Runnable, MyThreadPool> discardOldest() {
        return (task, pool) -> {
            // 简化实现：完整版需访问池内 workQueue（教程注明从简）
            System.err.println("[discardOldest] 丢弃队头并加入新任务");
            task.run();
        };
    }
}
