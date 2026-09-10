package com.orders.concurrent;

import java.util.ArrayList;
import java.util.List;
import java.util.Objects;
import java.util.Set;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.Callable;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.FutureTask;
import java.util.concurrent.Future;
import java.util.concurrent.RejectedExecutionException;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.locks.ReentrantLock;
import java.util.function.BiConsumer;

/**
 * 自实现线程池（教程 §07 全书最高峰）：七参数 + 4 拒绝策略 + 优雅关停
 *
 * 任务流转规则（经典面试题）：
 *   新任务 → 线程数 < core？→ 起核心线程
 *            ↓ 否
 *           队列没满？→ 入队等候
 *            ↓ 否
 *           线程数 < max？→ 起临时线程
 *            ↓ 否
 *           走拒绝策略
 *
 * ⚠️ 相比教程原版修复了 shutdown 挂起 bug：原版只置 volatile 标志不 interrupt，
 *    core worker 永久阻塞在 take() 上，JVM 无法退出。修复：shutdown 时
 *    interrupt 唤醒所有 worker，Worker 被唤醒后"队列跑完才退出"（drain 语义）。
 */
public class MyThreadPool {

    private final int corePoolSize;
    private final int maxPoolSize;
    private final long keepAliveMillis;
    private final BlockingQueue<Runnable> workQueue;
    private final BiConsumer<Runnable, MyThreadPool> rejectPolicy;

    private final Set<Worker> workers = ConcurrentHashMap.newKeySet();
    private final ReentrantLock mainLock = new ReentrantLock();
    private volatile boolean shutdown = false;        // ⭐ volatile 关停标志

    public MyThreadPool(int corePoolSize, int maxPoolSize,
                        long keepAliveMillis,
                        BlockingQueue<Runnable> workQueue,
                        BiConsumer<Runnable, MyThreadPool> rejectPolicy) {
        if (corePoolSize < 0 || maxPoolSize < corePoolSize) {
            throw new IllegalArgumentException("线程数参数非法");
        }
        this.corePoolSize    = corePoolSize;
        this.maxPoolSize     = maxPoolSize;
        this.keepAliveMillis = keepAliveMillis;
        this.workQueue       = Objects.requireNonNull(workQueue);
        this.rejectPolicy    = Objects.requireNonNull(rejectPolicy);
    }

    /** 内部 Worker：先执行首发任务，再循环从队列拉任务
     * ⭐⭐⭐ 关键 1：try-catch 包住任务执行——不让业务异常杀死 Worker（造 BUG #5 的修复）
     * ⭐⭐⭐ 关键 2：firstTask 由 Worker 直接持有（JDK 同款设计），不经队列——
     *              教程原版用 workQueue.offer(firstTask)，队列满时任务被静默丢弃 */
    private final class Worker implements Runnable {
        final Thread thread;
        final boolean isCore;       // 核心线程 take 永久阻塞；临时线程 poll 超时退出
        Runnable firstTask;

        Worker(Runnable firstTask, boolean isCore) {
            this.firstTask = firstTask;
            this.isCore = isCore;
            this.thread = new Thread(this, "MyPool-Worker-" + workerSeq.incrementAndGet());
            this.thread.setUncaughtExceptionHandler((t, e) ->
                    System.err.println("[MyThreadPool] 线程 " + t.getName() + " 未捕获异常: " + e));
        }

        @Override
        public void run() {
            Runnable task = firstTask;
            firstTask = null;
            try {
                while (task != null || !shutdown || !workQueue.isEmpty()) {
                    if (task == null) {
                        try {
                            task = isCore
                                    ? workQueue.take()
                                    : workQueue.poll(keepAliveMillis, TimeUnit.MILLISECONDS);
                            if (task == null) {
                                break;                  // 临时线程空闲超时 → 退出
                            }
                        } catch (InterruptedException e) {
                            // shutdown 被唤醒：队列跑完才退出（drain 语义）
                            if (shutdown && workQueue.isEmpty()) break;
                            Thread.interrupted();       // 清中断标志，继续 drain
                            continue;
                        }
                    }
                    try {
                        task.run();
                    } catch (RuntimeException ex) {     // ⭐ 不让异常杀死 Worker
                        System.err.println("[MyThreadPool] 任务异常: " + ex);
                    }
                    task = null;
                }
            } finally {
                workers.remove(this);                   // 退出时摘除登记
            }
        }
    }

    private final java.util.concurrent.atomic.AtomicInteger workerSeq =
            new java.util.concurrent.atomic.AtomicInteger();

    public void execute(Runnable task) {
        Objects.requireNonNull(task, "task 不能为 null");
        if (shutdown) {
            rejectPolicy.accept(task, this);
            return;
        }

        // 步骤 1：当前线程数 < core？→ 起核心线程
        if (workers.size() < corePoolSize) {
            if (addWorker(task, true)) return;
        }

        // 步骤 2：进队列等候
        if (workQueue.offer(task)) {
            // 二次检查：可能在入队前刚好关停
            if (shutdown && workQueue.remove(task)) {
                rejectPolicy.accept(task, this);
            }
            // 若当前 0 个 worker（极端：core=0），需补上
            else if (workers.isEmpty()) {
                addWorker(null, false);
            }
            return;
        }

        // 步骤 3：队列满了 → 起临时线程到 max
        if (workers.size() < maxPoolSize) {
            if (addWorker(task, false)) return;
        }

        // 步骤 4：max 也满了 → 走拒绝策略
        rejectPolicy.accept(task, this);
    }

    /** 创建并启动一个 Worker，可附带首发任务（直接持有，不进队列）*/
    private boolean addWorker(Runnable firstTask, boolean isCore) {
        mainLock.lock();
        try {
            int currentSize = workers.size();
            int limit       = isCore ? corePoolSize : maxPoolSize;
            if (currentSize >= limit) return false;

            Worker w = new Worker(firstTask, isCore);   // ⭐ 首发任务直接交给 Worker
            workers.add(w);
            w.thread.start();
            return true;
        } finally {
            mainLock.unlock();
        }
    }

    /** Callable 版：返回 Future */
    public <T> Future<T> submit(Callable<T> task) {
        FutureTask<T> ft = new FutureTask<>(task);
        execute(ft);
        return ft;
    }

    /** 优雅关停：不接新任务，把队列剩余的跑完
     * ⭐ interrupt 唤醒阻塞在 take/poll 上的 worker（教程原版缺失，导致 JVM 挂起）*/
    public void shutdown() {
        shutdown = true;
        for (Worker w : workers) {
            w.thread.interrupt();
        }
    }

    /** 立即关停：清队列 + 中断所有 worker */
    public List<Runnable> shutdownNow() {
        shutdown = true;
        List<Runnable> drained = new ArrayList<>();
        workQueue.drainTo(drained);
        for (Worker w : workers) {
            w.thread.interrupt();
        }
        return drained;
    }

    public boolean awaitTermination(long timeoutMillis) throws InterruptedException {
        long deadline = System.currentTimeMillis() + timeoutMillis;
        while (!workers.isEmpty()) {
            long remain = deadline - System.currentTimeMillis();
            if (remain <= 0) return false;
            Thread.sleep(Math.min(50, remain));
        }
        return true;
    }

    public int getActiveCount() { return workers.size(); }
    public int getQueueSize()   { return workQueue.size(); }
    public boolean isShutdown() { return shutdown; }

    /** 便捷构造：等价 JDK Executors.newFixedThreadPool */
    public static MyThreadPool newFixedPool(int n, BlockingQueue<Runnable> queue) {
        return new MyThreadPool(n, n, 0, queue, (t, p) -> {
            throw new RejectedExecutionException("任务被拒绝，活跃=" + p.getActiveCount());
        });
    }
}
