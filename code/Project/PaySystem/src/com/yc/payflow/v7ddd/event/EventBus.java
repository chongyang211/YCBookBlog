package com.yc.payflow.v7ddd.event;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * ============================================================
 *  EventBus 事件总线（对应博客第 11 篇「六边形架构 · 出站端口」）
 *  ------------------------------------------------------------
 *  【为什么放在这里而不是塞进 Order 聚合】：
 *    Order 聚合应该只负责一致性守卫，不负责事件路由。
 *    发布事件的能力抽成 EventBus 端口，由基础设施层实现。
 *
 *  【本教学场景用同步 in-process 实现】：
 *    生产可换成 Kafka/RabbitMQ 等异步 broker，业务代码零改动。
 * ============================================================
 */
public class EventBus {
    private final Map<Class<?>, List<EventSubscriber<?>>> registry = new HashMap<>();

    public <T extends DomainEvent> void subscribe(EventSubscriber<T> subscriber) {
        registry.computeIfAbsent(subscriber.subscribedTo(), k -> new ArrayList<>())
                .add(subscriber);
    }

    @SuppressWarnings({"unchecked", "rawtypes"})
    public void publish(DomainEvent event) {
        List<EventSubscriber<?>> subs = registry.getOrDefault(event.getClass(), List.of());
        for (EventSubscriber sub : subs) {
            try {
                sub.handle(event);
            } catch (Throwable t) {
                // 教学场景简化处理；生产建议：进入死信队列 + 告警
                System.err.println("[EventBus] 订阅方处理失败 "
                    + sub.getClass().getSimpleName() + ": " + t.getMessage());
            }
        }
    }
}
