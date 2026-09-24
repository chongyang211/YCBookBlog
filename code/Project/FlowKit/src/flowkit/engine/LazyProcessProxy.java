package flowkit.engine;

import flowkit.domain.Ticket;
import flowkit.notify.EventBus;

import java.util.function.Supplier;

/**
 * 懒加载代理 —— 控制"能不能/何时"访问（与装饰器"添新料"意图分家）。
 * 注册中心里放的是代理，秒启动；首单进来才付加载成本，调用方无感。
 */
public class LazyProcessProxy implements ProcessDefApi {
    private final String key;
    private final Supplier<ProcessDef> loader;
    private volatile ProcessDef real;               // 双重检测懒加载

    public LazyProcessProxy(String key, Supplier<ProcessDef> loader) {
        this.key = key;
        this.loader = loader;
    }

    @Override public String key() { return key; }

    @Override public ApprovalContext instantiate(Ticket t, EventBus bus) {
        return real().instantiate(t, bus);
    }

    private ProcessDef real() {
        if (real == null) {
            synchronized (this) {
                if (real == null) {
                    System.out.println("[懒加载] 首次加载流程 " + key);
                    real = loader.get();
                }
            }
        }
        return real;
    }
}
