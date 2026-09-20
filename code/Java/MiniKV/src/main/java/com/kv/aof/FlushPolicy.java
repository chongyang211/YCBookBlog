package com.kv.aof;

/**
 * AOF 刷盘策略（阶段⑦）—— 与 Redis appendfsync 三档完全一致。
 *
 * write() → 用户态缓冲（BufferedWriter）
 * flush() → OS 页缓存（内核态）
 * fsync() → 磁盘物理介质（FileChannel.force）
 *
 * 策略             fsync 时机       性能              安全
 * EVERY_WRITE      每条命令          最慢（约 4k QPS）  断电不丢
 * EVERY_SECOND     每秒（生产推荐）   中等（约 75k QPS） 最多丢 1 秒
 * NEVER            不主动，靠 OS     最快              最不安全
 */
public enum FlushPolicy {
    EVERY_WRITE,    // 每条命令 fsync（最安全 / 最慢）
    EVERY_SECOND,   // 每秒 fsync（生产推荐）
    NEVER           // 不主动（最快 / 最不安全）
}
