package com.orders.concurrent;

import java.util.LinkedList;
import java.util.Queue;

/** v1：故意用 if（虚假唤醒陷阱演示，教程 §6.1 造 BUG #4）
 * 仅供 QueueDemo 展示"消费者拿到 null"的错误现象——业务请用 MyBlockingQueue */
public class MyBlockingQueueV1<T> {

    private final Queue<T> queue = new LinkedList<>();
    private final int capacity;

    public MyBlockingQueueV1(int capacity) {
        if (capacity <= 0) throw new IllegalArgumentException("容量必须 > 0");
        this.capacity = capacity;
    }

    public synchronized void put(T item) throws InterruptedException {
        if (queue.size() == capacity) wait();    // ⚠️ if 错误！醒来不重查条件
        queue.offer(item);
        notifyAll();
    }

    public synchronized T take() throws InterruptedException {
        if (queue.isEmpty()) wait();             // ⚠️ if 错误！
        T item = queue.poll();
        notifyAll();
        return item;
    }

    public synchronized int size() { return queue.size(); }
}
