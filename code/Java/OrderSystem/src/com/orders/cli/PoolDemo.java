package com.orders.cli;

import com.orders.concurrent.MyThreadPool;
import com.orders.pool.RejectPolicy;

import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.Future;
import java.util.concurrent.atomic.AtomicInteger;

/**
 * 阶段⑥线程池演示（教程 §7.4）：
 * ① 七参数 + CallerRuns 拒绝策略 + 优雅关停
 * ② Worker 异常不饿死（教程造 BUG #5 的修复验证）
 */
public class PoolDemo {

    public static void main(String[] args) throws Exception {
        System.out.println("========== ① 基本流转：core=2 max=4 queue=10，20 任务 ==========");
        MyThreadPool pool = new MyThreadPool(
                2, 4, 5_000,
                new ArrayBlockingQueue<>(10),
                RejectPolicy.callerRuns());

        AtomicInteger completed = new AtomicInteger();
        for (int i = 0; i < 20; i++) {
            int id = i;
            pool.execute(() -> {
                try { Thread.sleep(100); } catch (InterruptedException e) { Thread.currentThread().interrupt(); }
                completed.incrementAndGet();
            });
        }

        pool.shutdown();
        boolean ok = pool.awaitTermination(10_000);
        System.out.println("完成 " + completed.get() + " / 20，awaitTermination=" + ok);

        System.out.println("\n========== ② Worker 异常不饿死（try-catch 修复验证）==========");
        MyThreadPool pool2 = new MyThreadPool(
                2, 4, 5_000,
                new ArrayBlockingQueue<>(100),
                RejectPolicy.callerRuns());

        AtomicInteger done2 = new AtomicInteger();
        for (int i = 0; i < 100; i++) {
            int id = i;
            pool2.execute(() -> {
                if (id % 2 == 0) throw new RuntimeException("故意抛 #" + id);
                done2.incrementAndGet();
            });
        }

        pool2.shutdown();
        pool2.awaitTermination(10_000);
        // 50 个正常任务必须全部执行（若 Worker 被异常杀死则大量任务堆积饿死）
        System.out.println("正常任务完成 " + done2.get() + " / 50 "
                + (done2.get() == 50 ? "（Worker 未被异常杀死）" : "（仍有饿死！）"));

        System.out.println("\n========== ③ submit 返回 Future ==========");
        MyThreadPool pool3 = new MyThreadPool(
                2, 2, 0,
                new ArrayBlockingQueue<>(10),
                RejectPolicy.callerRuns());
        Future<Integer> f = pool3.submit(() -> {
            Thread.sleep(100);
            return 42;
        });
        System.out.println("submit 结果: " + f.get());
        pool3.shutdown();
        pool3.awaitTermination(5_000);
    }
}
