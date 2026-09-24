package com.yc.payflow.v7ddd.event;

/**
 * EventSubscriber 事件订阅方——每个上下文的独立入口。
 *
 * 【设计选择】：为什么泛型 T extends DomainEvent 而不是所有事件走一个 handle？
 *   ① 编译期约束，订阅错事件类型直接报错
 *   ② 每个订阅方职责单一（SRP）
 */
public interface EventSubscriber<T extends DomainEvent> {
    Class<T> subscribedTo();
    void handle(T event);
}
