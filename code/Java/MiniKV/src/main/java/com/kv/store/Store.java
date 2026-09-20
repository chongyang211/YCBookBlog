package com.kv.store;

import java.util.Map;
import java.util.Optional;
import java.util.Set;
import java.util.TreeSet;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentSkipListMap;
import java.util.concurrent.atomic.AtomicLong;

/**
 * 数据存储核心（阶段③ → 阶段⑨ → 阶段⑩ 的最终形态）。
 *
 * 演进路径：
 *   阶段③ HashMap（100 线程并发会 CME / 丢数据 → 见 ConcurrencyDemo）
 *   阶段⑨ ConcurrentHashMap + compute 原子复合操作 + remove(k,v) CAS 删除
 *   阶段⑩ ConcurrentSkipListMap 过期索引 + sweepExpired 主动清理
 *
 * 设计要点：
 *   · CHM 解决"单 key 原子性"——compute 在同一次桶锁内完成"读旧值 → 造新 Entry → 写回"
 *   · remove(key, expectedEntry) 是 CAS 风格删除：值还是当初读到的那个才删，避免误删别的线程刚写的新值
 *   · expireIndex 按 expireAtMs 排序，sweepExpired 用 headMap 一次取出所有到期桶（O(log N)）
 */
public class Store {

    private final ConcurrentHashMap<String, Entry> data = new ConcurrentHashMap<>();

    /** 按到期时间排序：key=expireAtMs，value=该时刻到期的 key 集合 */
    private final ConcurrentSkipListMap<Long, Set<String>> expireIndex = new ConcurrentSkipListMap<>();

    private final AtomicLong version = new AtomicLong();

    public void set(String key, Value v) {
        // ⭐ compute：保证"读旧值 + 创建新 Entry"在同一次桶锁内原子完成
        data.compute(key, (k, old) -> {
            long ver = version.incrementAndGet();
            return (old == null)
                    ? new Entry(v, Optional.empty(), ver)
                    : new Entry(v, old.expireAtMs(), ver);
        });
    }

    /** 惰性删除：GET 时才检查过期（与 Redis 双策略之一） */
    public Optional<Value> get(String key) {
        Entry e = data.get(key);
        if (e == null) return Optional.empty();
        if (e.isExpired(System.currentTimeMillis())) {
            // ⭐ CAS 删除：避免误删别的线程刚 SET 的新值
            data.remove(key, e);
            removeExpireIndex(key, e);
            return Optional.empty();
        }
        return Optional.of(e.value());
    }

    public boolean del(String key) {
        Entry removed = data.remove(key);
        if (removed != null) removeExpireIndex(key, removed);
        return removed != null;
    }

    public boolean expire(String key, long seconds) {
        Entry old = data.get(key);
        if (old == null) return false;
        long expireAt = System.currentTimeMillis() + seconds * 1000;

        // 移除旧索引（若有）
        removeExpireIndex(key, old);

        // 写新 Entry + 新索引
        data.put(key, old.withExpireAt(expireAt));
        expireIndex.computeIfAbsent(expireAt, k -> ConcurrentHashMap.newKeySet()).add(key);
        return true;
    }

    private void removeExpireIndex(String key, Entry e) {
        e.expireAtMs().ifPresent(ts -> {
            Set<String> keys = expireIndex.get(ts);
            if (keys != null) {
                keys.remove(key);
                if (keys.isEmpty()) expireIndex.remove(ts);
            }
        });
    }

    /** 守护线程调用：扫一次过期表（主动删除，与惰性删除组成双策略） */
    public int sweepExpired() {
        long now = System.currentTimeMillis();
        // ⭐ headMap(0, now] 一次性取出所有到期的——O(log N)
        Map<Long, Set<String>> dueMap = expireIndex.headMap(now, true);

        int removed = 0;
        for (Map.Entry<Long, Set<String>> bucket : dueMap.entrySet()) {
            for (String key : bucket.getValue()) {
                Entry e = data.get(key);
                // 二次检查：可能在这期间被 EXPIRE 续期了
                if (e != null && e.isExpired(now)) {
                    data.remove(key, e);            // CAS 删除
                    removed++;
                }
            }
        }
        dueMap.clear();                             // 清空已过期的索引桶
        return removed;
    }

    /** CHM 的迭代器是弱一致性的，不会抛 CME */
    public Set<String> keys() {
        return new TreeSet<>(data.keySet());        // 排序输出更友好
    }

    public int size() { return data.size(); }
}
