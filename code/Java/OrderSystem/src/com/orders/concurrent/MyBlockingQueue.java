package com.orders.concurrent;

import java.util.LinkedList;
import java.util.Queue;

/** v2：正确版——wait 永远写在 while 循环里（教程 §6.2 修复）
 * 防虚假唤醒 + 防多消费者竞争：醒来必须重新检查条件 */
public class MyBlockingQueue<T> {

    private final Queue<T> queue = new LinkedList<>();
    private final int capacity;

    public MyBlockingQueue(int capacity) {
        if (capacity <= 0) throw new IllegalArgumentException("容量必须 > 0");
        this.capacity = capacity;
    }

    public synchronized void put(T item) throws InterruptedException {
        while (queue.size() == capacity) wait();   // ✅ while 循环检查
        queue.offer(item);
        notifyAll();
    }

    public synchronized T take() throws InterruptedException {
        while (queue.isEmpty()) wait();            // ✅ while
        T item = queue.poll();
        notifyAll();
        return item;
    }

    public synchronized int size() { return queue.size(); }
}
