package com.orders.cli;

import com.orders.concurrent.MyBlockingQueue;
import com.orders.concurrent.MyBlockingQueueV1;

/**
 * 阶段⑤虚假唤醒演示（教程 §6.2 造 BUG #4）：
 * V1（if 写法）双消费者抢 1 个元素 → 可能有人拿到 null
 * 正确版（while）永不返回 null
 */
public class QueueDemo {

    public static void main(String[] args) throws InterruptedException {
        System.out.println("========== V1（if 写法，可能有 BUG）==========");
        demoV1();
        Thread.sleep(200);

        System.out.println("\n========== V2（while 写法，正确）==========");
        demoV2();
        Thread.sleep(200);
    }

    private static void demoV1() throws InterruptedException {
        MyBlockingQueueV1<Integer> q = new MyBlockingQueueV1<>(2);

        for (int i = 0; i < 2; i++) {
            int idx = i;
            Thread c = new Thread(() -> {
                try {
                    Integer v = q.take();
                    System.out.println("消费者 " + idx + " 拿到: " + v);
                } catch (InterruptedException e) {
                    Thread.currentThread().interrupt();
                }
            }, "Consumer-" + i);
            c.setDaemon(true);     // ⭐ 演示线程不阻止 JVM 退出（第二个消费者会永远阻塞）
            c.start();
        }

        Thread.sleep(100);         // 让消费者先 wait

        // 生产 1 个 → notifyAll 唤醒 2 个消费者：
        // 消费者 0 抢到锁拿走唯一元素；消费者 1 醒来若不重查条件（if 版）→ poll 返回 null
        Thread p = new Thread(() -> {
            try { q.put(42); }
            catch (InterruptedException e) { Thread.currentThread().interrupt(); }
        }, "Producer");
        p.setDaemon(true);
        p.start();

        Thread.sleep(500);
    }

    private static void demoV2() throws InterruptedException {
        MyBlockingQueue<Integer> q = new MyBlockingQueue<>(2);

        for (int i = 0; i < 2; i++) {
            int idx = i;
            Thread c = new Thread(() -> {
                try {
                    Integer v = q.take();
                    System.out.println("消费者 " + idx + " 拿到: " + v);
                } catch (InterruptedException e) {
                    Thread.currentThread().interrupt();
                }
            }, "Consumer-" + i);
            c.setDaemon(true);
            c.start();
        }

        Thread.sleep(100);

        Thread p = new Thread(() -> {
            try { q.put(42); }
            catch (InterruptedException e) { Thread.currentThread().interrupt(); }
        }, "Producer");
        p.setDaemon(true);
        p.start();

        Thread.sleep(500);
    }
}
