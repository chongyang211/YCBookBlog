package flowkit.engine;

import flowkit.domain.Ticket;
import flowkit.notify.EventBus;

/**
 * 流程定义对外接口 —— 懒加载代理与真实定义的共同抽象。
 * 注册中心里放的是接口，调用方无感"是真身还是代理"。
 */
public interface ProcessDefApi {
    String key();
    ApprovalContext instantiate(Ticket t, EventBus bus);
}
