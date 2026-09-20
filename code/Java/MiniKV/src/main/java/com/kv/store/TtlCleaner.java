package com.kv.store;

import com.kv.log.Log;

/**
 * TTL 主动清理守护线程（阶段⑩）。
 *
 * 优雅关停三件套（缺一不可）：
 *   1. volatile boolean running —— 保证主线程的 stop 信号对守护线程可见
 *      （造 BUG #5：去掉 volatile，JIT 把 running 缓存到寄存器 → 永远停不下来）
 *   2. interrupt() —— 打断 Thread.sleep，立刻响应关停
 *   3. join(timeout) —— 主线程等待退出，超时可告警
 *
 * 扫描异常不退出线程：记录后继续，避免一次扫描挂掉就永久失效。
 */
public class TtlCleaner extends Thread {

    private final Store store;
    private final long sweepIntervalMs;
    private volatile boolean running = true;        // ⭐ volatile 关停标志

    public TtlCleaner(Store store, long sweepIntervalMs) {
        super("TtlCleaner");
        this.store = store;
        this.sweepIntervalMs = sweepIntervalMs;
        setDaemon(true);                            // ⭐ 守护线程：不阻止 JVM 退出
    }

    @Override
    public void run() {
        Log.info("[TTL] 守护线程启动，扫描间隔 %d ms", sweepIntervalMs);
        while (running && !Thread.currentThread().isInterrupted()) {
            try {
                int removed = store.sweepExpired();
                if (removed > 0) {
                    Log.debug("[TTL] 本轮清理 %d 个过期 key", removed);
                }
                Thread.sleep(sweepIntervalMs);
            } catch (InterruptedException e) {
                Log.info("[TTL] 收到中断，退出守护线程");
                Thread.currentThread().interrupt();
                break;
            } catch (Exception e) {
                Log.error("[TTL] 扫描异常: %s", e.getMessage());
                // ⚠️ 不退出！记录后继续——避免一次扫描挂掉就永久失效
            }
        }
        Log.info("[TTL] 守护线程已退出");
    }

    /** 优雅停止 */
    public void shutdownGracefully() {
        running = false;
        interrupt();                                // 打断当前 sleep
    }
}
