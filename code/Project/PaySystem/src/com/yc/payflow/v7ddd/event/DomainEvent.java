package com.yc.payflow.v7ddd.event;

import java.time.Instant;

/**
 * ============================================================
 *  DomainEvent 领域事件基类（对应博客第 11 篇「战术四件套 · 领域事件」）
 *  ------------------------------------------------------------
 *  【职责】：
 *    - 描述"发生了什么"，而非"需要谁做什么"（后者是命令）
 *    - 携带足够上下文让订阅方独立处理
 *    - 时间戳与 ID 便于追溯/重放
 *
 *  【为什么用领域事件】：
 *    结算成功后要触发：库存扣减、发票开具、积分发放、账单推送、风控埋点...
 *    ① 如果都写在 checkout() 里 → 上帝方法，散弹式修改
 *    ② 如果每处都 new SmsClient().send() → 硬耦合，加订阅方要改结算
 *    ③ 发布事件 → 结算只关心"我完成了" → 订阅方按需订阅 → 零耦合扩展
 * ============================================================
 */
public abstract class DomainEvent {
    private final String eventId;
    private final Instant occurredAt;

    protected DomainEvent() {
        this.eventId = java.util.UUID.randomUUID().toString();
        this.occurredAt = Instant.now();
    }

    public String eventId() { return eventId; }
    public Instant occurredAt() { return occurredAt; }
    public String eventType() { return getClass().getSimpleName(); }
}
