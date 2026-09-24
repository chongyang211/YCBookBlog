package flowkit.notify;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

/**
 * 异步版事件总线 —— 复用 SyncEventBus 的注册表，只换执行模型。
 * 通知失败只告警，不阻断审批主流程（隔离）。
 * 取舍：异步解决了阻塞，但引入"通知乱序/丢失"，生产答案是本地消息表 + MQ。
 */
public class AsyncEventBus extends SyncEventBus {
    private final ExecutorService pool =
        Executors.newFixedThreadPool(4, r -> {
            Thread th = new Thread(r, "notify-worker");
            th.setDaemon(true);
            return th;
        });

    @Override public void publish(String event, flowkit.domain.Ticket t) {
        for (var l : listenersOf(event)) {
            pool.submit(() -> {
                try { l.accept(t); }
                catch (Exception e) {
                    System.out.println("[通知失败] " + e.getMessage() + "（不阻断主流程）");
                }
            });
        }
    }
}
