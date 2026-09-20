package com.kv.store;

import java.util.Optional;

/**
 * 一个 KV 表项（阶段③）。
 * - value：当前值
 * - expireAtMs：过期时间点（epoch ms）；空表示永不过期
 * - version：版本号（阶段⑨ 并发 set 用 AtomicLong 递增）
 */
public record Entry(Value value, Optional<Long> expireAtMs, long version) {

    public static Entry of(Value v) {
        return new Entry(v, Optional.empty(), 0L);
    }

    public Entry withValue(Value v) {
        return new Entry(v, this.expireAtMs, this.version + 1);
    }

    public Entry withExpireAt(long epochMs) {
        return new Entry(this.value, Optional.of(epochMs), this.version + 1);
    }

    public boolean isExpired(long nowMs) {
        return expireAtMs.map(ts -> ts <= nowMs).orElse(false);
    }
}
