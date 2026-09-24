package flowkit.notify;

import flowkit.domain.Ticket;

import java.util.function.Consumer;

/**
 * 观察者 —— 审批主流程只发事件，不认识任何通知逻辑。
 * 观察者是"一对多广播，不关心谁处理"（与职责链"有序接力、责任唯一"语义不同）。
 */
public interface EventBus {
    void on(String event, Consumer<Ticket> listener);
    void publish(String event, Ticket t);
}
