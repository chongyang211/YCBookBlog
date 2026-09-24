package flowkit.domain;

import flowkit.engine.ApprovalContext;

/**
 * 状态模式接口 —— 每个状态一个类，自己决定"在我这个状态下，什么能做、做了去哪"。
 * 转移规则从"散落在 N 个命令方法里的 if-else"内聚到 5 个状态类里。
 */
public interface TicketState {
    TicketStatus name();
    void submit(Ticket t);
    void approve(Ticket t, ApprovalContext ctx);   // ctx 由职责链提供
    void reject(Ticket t, String reason);
    void withdraw(Ticket t);
}
