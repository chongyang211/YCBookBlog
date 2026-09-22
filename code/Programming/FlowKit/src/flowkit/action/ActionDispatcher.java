package flowkit.action;

import flowkit.audit.ActionLog;
import flowkit.domain.Ticket;
import flowkit.engine.ApprovalContext;
import flowkit.history.AuditEvent;

import java.time.Instant;
import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.Deque;
import java.util.List;

/**
 * Caretaker —— 只管存取，不拆盒。
 * dispatch 三步曲：① 先拍快照 ② 再执行 ③ 落审计。
 * undo 两步曲：结构性回滚（命令自己知道怎么拆）+ 数据性回滚（恢复快照）。
 */
public class ActionDispatcher implements TaskActionDispatcher {
    private final Deque<Snapshot> history = new ArrayDeque<>();
    private record Snapshot(TaskAction action, TicketMemento before) {}

    private final ActionLog log;
    private final List<AuditEvent> liveEvents = new ArrayList<>();

    public ActionDispatcher(ActionLog log) { this.log = log; }

    @Override
    public void dispatch(TaskAction action, Ticket t, ApprovalContext ctx) {
        history.push(new Snapshot(action, t.snapshot()));   // ① 先拍快照
        action.execute(t, ctx);                             // ② 再执行
        log.write(action.describe());                       // ③ 落审计（命令日志）
        liveEvents.add(new AuditEvent(Instant.now(), action.operator(),
            action.describe().split(" ")[0], t.getId()));
    }

    @Override
    public void undo(Ticket t, ApprovalContext ctx) {
        if (history.isEmpty()) {
            System.out.println("没有可撤销的动作");
            return;
        }
        Snapshot s = history.pop();
        s.action().undo(t, ctx);       // 结构性回滚（拔节点/回退节点）
        t.restore(s.before());         // 数据性回滚（恢复快照）
        System.out.println("[OK] 已撤销: " + s.action().describe());
    }

    /** 供迭代器归并的历史事件（内存侧数据源）。 */
    public List<AuditEvent> liveEvents() { return List.copyOf(liveEvents); }
}
