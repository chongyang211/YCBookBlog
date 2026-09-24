package flowkit.engine;

import flowkit.domain.Ticket;
import flowkit.domain.TicketStatus;
import flowkit.notify.EventBus;

import java.time.Duration;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

/**
 * 超时催办装饰器 —— 不改节点类，给任意节点"穿"上超时提醒。
 * 与被包裹者同接口，可层层叠加（再包一层抄送也行）。
 */
public class TimeoutDecorator extends NodeDecorator {
    private static final ScheduledExecutorService TIMER = Executors.newScheduledThreadPool(1, r -> {
        Thread th = new Thread(r, "flowkit-timeout");
        th.setDaemon(true);
        return th;
    });

    private final Duration threshold;
    private final EventBus bus;

    public TimeoutDecorator(NodeDef inner, Duration threshold, EventBus bus) {
        super(inner);                                   // 持有被包裹者
        this.threshold = threshold;
        this.bus = bus;
    }

    @Override public void enter(Ticket t, ApprovalContext ctx) {
        super.enter(t, ctx);                            // ① 先干原活
        ApproverNode node = ctx.currentNode();
        TIMER.schedule(() -> {                          // ② 再加料：超时未批 → 发事件
            if (ctx.currentNode() == node && !node.isDone()
                && t.getStatus() == TicketStatus.REVIEWING) {
                bus.publish("NODE_TIMEOUT", t);         // 触发催办通知（观察者）
            }
        }, threshold.toMillis(), TimeUnit.MILLISECONDS);
    }
}
