package flowkit.facade;

import flowkit.domain.Ticket;
import flowkit.domain.TicketStatus;
import flowkit.engine.ApprovalContext;
import flowkit.facade.service.FormService;
import flowkit.facade.service.NotifyService;
import flowkit.facade.service.ProcessService;
import flowkit.facade.service.TaskService;
import flowkit.notify.EventBus;

/**
 * 中介者 —— 网状变星型：四方（表单/流程/任务/通知）只识主持人。
 * 纪律：只做"编排"（谁先谁后），不做任何业务；
 * 一旦中介者里长出 if-else 业务逻辑，它正在变成上帝类。
 */
public class FlowMediator {
    private final FormService form;
    private final ProcessService process;
    private final TaskService task;
    private final NotifyService notify;
    private final EventBus bus;

    public FlowMediator(FormService form, ProcessService process, TaskService task,
                        NotifyService notify, EventBus bus) {
        this.form = form;
        this.process = process;
        this.task = task;
        this.notify = notify;
        this.bus = bus;
    }

    /** 提交后的四方联动。 */
    public void onSubmitted(Ticket t, ApprovalContext ctx) {
        form.lock(t);                                // 表单锁定，审批中不可改
        task.createPending(t, ctx.currentNode());    // 生成待办任务
        System.out.println("    -> 流转到: " + ctx.currentNode().nodeName()
            + " (" + ctx.currentNode().currentApprover(t) + ")");
        notify.sendTodo(ctx.currentNode().currentApprover(t), t);
    }

    /** 每次审批动作后的联动：终审 → 广播结果事件；未完 → 提醒下一审批人。 */
    public void onApproved(Ticket t, ApprovalContext ctx) {
        if (t.getStatus() == TicketStatus.APPROVED) {
            bus.publish("TICKET_APPROVED", t);       // 观察者广播（邮件/积分/归档各自订阅）
        } else if (ctx.currentNode() instanceof flowkit.engine.ConditionNode) {
            // 条件节点已自动路由，无需待办
        } else if (ctx.currentNode() != null) {
            notify.sendTodo(ctx.currentNode().currentApprover(t), t);
        }
    }

    /** 撤回的编排规则集中在一处：谁先谁后，谁都不能漏。 */
    public void withdraw(Ticket t, ApprovalContext ctx) {
        form.unlock(t);
        process.resetToHead(ctx);
        task.cancelAll(t, "工单已撤回");
        notify.send(t.getApplicant(), "撤回成功", "工单#" + t.getId() + " 已回到草稿，可修改后重新提交");
    }

    /** 驳回同理——每条规则一处编排。 */
    public void onRejected(Ticket t, String reason) {
        process.reject(t);
        task.cancelPending(t, reason);
        form.unlock(t);
        notify.send(t.getApplicant(), "驳回通知", "工单#" + t.getId() + " 被驳回: " + reason);
    }
}
