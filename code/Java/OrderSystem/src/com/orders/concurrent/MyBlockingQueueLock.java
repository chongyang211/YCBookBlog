package com.orders.concurrent;

import java.util.LinkedList;
import java.util.Queue;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.ReentrantLock;

/** v3：ReentrantLock + 双 Condition（教程 §6.3）
 * notFull / notEmpty 分开等待集合——signal 只唤醒该唤醒的一边 */
public class MyBlockingQueueLock<T> {

    private final Queue<T> queue = new LinkedList<>();
    private final int capacity;
    private final ReentrantLock lock = new ReentrantLock();
    private final Condition notFull  = lock.newCondition();  // 生产者等这个
    private final Condition notEmpty = lock.newCondition();  // 消费者等这个

    public MyBlockingQueueLock(int capacity) {
        if (capacity <= 0) throw new IllegalArgumentException("容量必须 > 0");
        this.capacity = capacity;
    }

    public void put(T item) throws InterruptedException {
        lock.lock();
        try {
            while (queue.size() == capacity) notFull.await();
            queue.offer(item);
            notEmpty.signal();              // ⭐ 只唤醒消费者，不唤醒其他生产者
        } finally {
            lock.unlock();                  // ⭐ ReentrantLock 不会自动解锁
        }
    }

    public T take() throws InterruptedException {
        lock.lock();
        try {
            while (queue.isEmpty()) notEmpty.await();
            T item = queue.poll();
            notFull.signal();               // ⭐ 只唤醒生产者
            return item;
        } finally {
            lock.unlock();
        }
    }

    public int size() {
        lock.lock();
        try { return queue.size(); }
        finally { lock.unlock(); }
    }
}
