package flowkit.notify;

import flowkit.domain.Ticket;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.function.Consumer;

/**
 * 同步版事件总线 —— 简单直接，先跑起来。
 * 缺陷：一个渠道网关超时，整条通知链卡死，审批主流程被拖挂（§09 的坑）。
 */
public class SyncEventBus implements EventBus {
    private final Map<String, List<Consumer<Ticket>>> listeners = new HashMap<>();

    @Override public void on(String event, Consumer<Ticket> l) {
        listeners.computeIfAbsent(event, k -> new ArrayList<>()).add(l);
    }

    @Override public void publish(String event, Ticket t) {
        for (Consumer<Ticket> l : listenersOf(event)) {
            l.accept(t);                                // ⚠️ 同步逐个执行，一个卡全卡
        }
    }

    /** 供异步子类复用注册表（只换执行模型，注册逻辑不重写）。 */
    protected List<Consumer<Ticket>> listenersOf(String event) {
        return listeners.getOrDefault(event, List.of());
    }
}
