package flowkit.facade;

import flowkit.action.ActionDispatcher;
import flowkit.action.AddSignCommand;
import flowkit.action.ApproveCommand;
import flowkit.action.RejectCommand;
import flowkit.action.SubmitCommand;
import flowkit.action.TaskAction;
import flowkit.action.TaskActionDispatcher;
import flowkit.audit.ActionLog;
import flowkit.audit.AuditProxy;
import flowkit.domain.Attachment;
import flowkit.domain.Ticket;
import flowkit.engine.ApprovalContext;
import flowkit.engine.ApproverNode;
import flowkit.engine.GroupNode;
import flowkit.engine.LazyProcessProxy;
import flowkit.engine.ProcessDef;
import flowkit.engine.ProcessDefApi;
import flowkit.engine.ProcessRegistry;
import flowkit.facade.service.FormService;
import flowkit.facade.service.NotifyService;
import flowkit.facade.service.ProcessService;
import flowkit.facade.service.TaskService;
import flowkit.history.AuditEvent;
import flowkit.history.TicketHistory;
import flowkit.notify.AsyncEventBus;
import flowkit.notify.DingTalkAdapter;
import flowkit.notify.EventBus;
import flowkit.notify.ResultNotification;
import flowkit.notify.SmtpAdapter;
import flowkit.notify.TimeoutNotification;
import flowkit.rule.ExprCache;
import flowkit.rule.RuleContext;
import flowkit.stats.AuditVisitor;
import flowkit.stats.FinanceVisitor;
import flowkit.stats.SlmVisitor;
import flowkit.task.AndTaskGroup;
import flowkit.task.SingleTask;
import flowkit.ui.IconPool;

import java.io.IOException;
import java.io.UncheckedIOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardOpenOption;
import java.time.Instant;
import java.time.YearMonth;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.util.stream.Collectors;

/**
 * 外观 —— 给 REPL/Controller 一个一站式入口。
 * 一个方法背后是一整场编排：建造者造单 → 工厂铺链 → 命令留痕 → 中介者联动四方。
 * main 从"业务代码"退位成"纯壳"。
 */
public class WorkflowFacade {
    private static final Pattern SUBMIT_LINE =
        Pattern.compile("SUBMIT (\\S+) (\\d+) \"(.*)\" by (\\S+)");

    private final EventBus bus;
    private final ActionLog actionLog;
    private final ActionDispatcher rawDispatcher;      // 真身（供取 liveEvents）
    private final TaskActionDispatcher dispatcher;     // 动态代理包裹后的审计版
    private final ProcessRegistry registry;
    private final FlowMediator mediator;

    private final Map<Long, ApprovalContext> ctxs = new HashMap<>();
    private Ticket current;                            // REPL 演示：单在途工单

    public WorkflowFacade() throws IOException {
        this.bus = new AsyncEventBus();
        this.actionLog = new ActionLog(Path.of("data/action.log"));
        this.rawDispatcher = new ActionDispatcher(actionLog);
        this.dispatcher = AuditProxy.wrap(rawDispatcher);          // 动态代理：全接口自动留痕
        this.registry = ProcessRegistry.getInstance();
        this.mediator = new FlowMediator(
            new FormService(), new ProcessService(), new TaskService(),
            new NotifyService(new DingTalkAdapter()), bus);
        registerListeners();
    }

    private void registerListeners() {
        // 终审通过 → 广播：邮件结果通知 + 积分 + 归档（各自订阅，主流程不认识它们）
        bus.on("TICKET_APPROVED", t -> {
            new ResultNotification(new SmtpAdapter()).notify(t.getApplicant(), t);
            System.out.println("[副作用] 已为 " + t.getApplicant() + " 加 10 积分；工单已归档");
        });
        // 节点超时（装饰器发出）→ 催办通知
        bus.on("NODE_TIMEOUT", t -> {
            ApprovalContext c = ctxs.get(t.getId());
            if (c != null && c.currentNode() != null) {
                new TimeoutNotification(new SmtpAdapter())
                    .notify(c.currentNode().currentApprover(t), t);
            }
        });
    }

    // ---------- 提交 ----------
    public long submit(String type, long amount, String reason) {
        Ticket t = internalSubmit(type, amount, reason, "yangc");
        dispatcher.dispatch(new SubmitCommand(t), t, ctxs.get(t.getId()));   // 命令留痕
        mediator.onSubmitted(t, ctxs.get(t.getId()));                        // 中介者联动四方
        return t.getId();
    }

    /** 重放用：只重建状态（工单 + 链 + 状态机），不写日志、不触发通知——AOF 重放不重复副作用。 */
    private Ticket internalSubmit(String type, long amount, String reason, String applicant) {
        Ticket t = Ticket.builder()                     // 建造者造单
            .type(type).amount(amount).reason(reason).applicant(applicant).build();
        ApprovalContext ctx = registry.get(type).instantiate(t, bus);   // 工厂铺链（单例注册中心）
        ctxs.put(t.getId(), ctx);
        this.current = t;
        System.out.println("[OK] 工单#" + t.getId() + " 已提交（流程=" + type + "）");
        return t;
    }

    // ---------- 审批动作（全部命令化） ----------
    public void approve(String operator) {
        Ticket t = requireTicket();
        dispatcher.dispatch(new ApproveCommand(operator), t, ctxOf(t));
        mediator.onApproved(t, ctxOf(t));
    }

    public void reject(String operator, String reason) {
        Ticket t = requireTicket();
        dispatcher.dispatch(new RejectCommand(operator, reason), t, ctxOf(t));
        mediator.onRejected(t, reason);
    }

    public void addsign(String person) {
        Ticket t = requireTicket();
        dispatcher.dispatch(new AddSignCommand(person), t, ctxOf(t));
    }

    public void undo() {
        Ticket t = requireTicket();
        dispatcher.undo(t, ctxOf(t));
    }

    // ---------- 撤回（状态机 + 中介者四方联动） ----------
    public void withdraw() {
        Ticket t = requireTicket();
        t.withdraw();                                   // REVIEWING → DRAFT
        mediator.withdraw(t, ctxOf(t));
    }

    // ---------- 附件（备忘录坑的道具：undo 后快照恢复它） ----------
    public void attach(String filename) {
        Ticket t = requireTicket();
        t.addAttachment(new Attachment(filename, Instant.now()));
        System.out.println("[OK] 已附加凭证: " + filename + "（当前附件数=" + t.getAttachments().size() + "）");
    }

    // ---------- 条件 DSL ----------
    public void expr(String source) {
        var e = ExprCache.of(source);                   // 第二次求值同一表达式走缓存
        System.out.println("AST: " + e);
        Ticket t = current;
        if (t != null) {
            System.out.println("求值: " + e.eval(RuleContext.of(t))
                + "（当前单 amount=" + t.getAmount() / 100 + "元）");
        }
        System.out.println("(AST 缓存内表达式数=" + ExprCache.size() + ")");
    }

    // ---------- 多渠道通知（适配器 × 桥接手动演示） ----------
    public void notifyAllChannels() {
        Ticket t = requireTicket();
        new ResultNotification(new DingTalkAdapter()).notify(t.getApplicant(), t);
        new ResultNotification(new flowkit.notify.WeComAdapter()).notify(t.getApplicant(), t);
        new ResultNotification(new SmtpAdapter()).notify(t.getApplicant(), t);
    }

    // ---------- 三视角统计（访问者） ----------
    public void stats() {
        Ticket t = requireTicket();
        ApprovalContext ctx = ctxOf(t);
        // 把当前审批链转成任务树：叶子 = 每个节点的审批任务
        // 金额是工单级属性，挂在链头任务上（避免逐节点重复累计）
        AndTaskGroup root = new AndTaskGroup();
        boolean head = true;
        for (ApproverNode n : ctx.chainNodes()) {
            long amt = head ? t.getAmount() : 0L;
            head = false;
            if (n instanceof GroupNode g) {
                for (SingleTask m : g.members()) {                   // 会签/或签：树内成员展开
                    root.add(new SingleTask(m.assignee(), amt, m.isApproved()));
                }
            } else {
                root.add(new SingleTask(n.nodeName(), amt, n.isDone()));
            }
        }
        FinanceVisitor fin = new FinanceVisitor();
        AuditVisitor aud = new AuditVisitor();
        SlmVisitor slm = new SlmVisitor();
        root.accept(fin); root.accept(aud); root.accept(slm);   // 一棵树喂三个视角
        System.out.println("[统计] 金额汇总=" + fin.totalAmount() + "分  "
            + aud.report() + "  节点P95耗时=" + slm.p95() + "ms");
    }

    // ---------- 历史时间线（迭代器：双源归并） ----------
    public void history() {
        Ticket t = requireTicket();
        TicketHistory h = new TicketHistory(rawDispatcher.liveEvents(), Path.of("data/history"));
        System.out.println("---- 审批全史（内存 + 归档归并）----");
        for (AuditEvent e : h) {                          // for-each 背后是 TimelineIterator
            System.out.printf("%s  %-10s %-8s #%d%n",
                e.time(), e.operator(), e.action(), e.ticketId());
        }
    }

    // ---------- 崩溃恢复（命令日志重放） ----------
    public void replay() {
        Path log = Path.of("data/action.log");
        if (!Files.exists(log)) {
            System.out.println("没有命令日志可重放");
            return;
        }
        int n = 0;
        try {
            for (String line : Files.readAllLines(log)) {
                if (line.isBlank()) continue;             // 容忍末尾半行损坏：跳过空行
                Matcher m = SUBMIT_LINE.matcher(line);
                if (m.matches()) {
                    // SUBMIT 行：重建工单与审批链（不写日志、不触发通知）
                    internalSubmit(m.group(1), Long.parseLong(m.group(2)), m.group(3), m.group(4));
                    current.submit();                  // 状态机推进 DRAFT → REVIEWING
                    current.touchSubmit();
                } else {
                    TaskAction action = ActionLog.parse(line);    // 文本 → 命令对象
                    action.execute(current, ctxOf(current));      // 逐条重放
                }
                System.out.println("[重放] " + line);
                n++;
            }
        } catch (IOException e) {
            throw new UncheckedIOException(e);
        }
        if (n > 0) {
            ApprovalContext ctx = ctxOf(current);
            System.out.println("[OK] " + n + " 条命令重放完毕，工单恢复：当前节点="
                + (ctx != null && ctx.currentNode() != null ? ctx.currentNode().nodeName() : "-"));
        } else {
            System.out.println("日志为空，无需重放");
        }
    }

    // ---------- 流程定义展示（享元图标池） ----------
    public void flow(String key) {
        ProcessDefApi api = registry.get(key);
        if (api instanceof LazyProcessProxy) {
            System.out.println("流程 " + key + " = 懒加载代理（首次 submit 时才真正加载）");
            return;
        }
        ProcessDef def = (ProcessDef) api;
        System.out.println("流程 " + key + " (v" + def.version() + ")  节点数=" + def.nodes().size());
        int x = 10;
        for (var node : def.nodes()) {
            IconPool.of(node.type()).render(x, 10, node.name());   // 同类型图标第二次必 HIT
            x += 20;
        }
    }

    // ---------- 状态展示 ----------
    public void status() {
        Ticket t = requireTicket();
        ApprovalContext ctx = ctxOf(t);
        System.out.println("工单#" + t.getId() + "  类型=" + t.getType()
            + "  金额=" + t.getAmount() + "分  状态=" + t.getStatus());
        boolean inFlight = t.getStatus() == flowkit.domain.TicketStatus.REVIEWING;
        if (inFlight && ctx != null && ctx.currentNode() != null) {
            System.out.println("  当前节点: " + ctx.currentNode().nodeName()
                + " (" + ctx.currentNode().currentApprover(t) + ")");
            if (ctx.currentNode() instanceof GroupNode g) {
                System.out.println(g.prettyTree());
            }
        }
        if (!t.getAttachments().isEmpty()) {
            System.out.println("  附件: " + t.getAttachments().stream()
                .map(Attachment::filename).collect(Collectors.joining(", ")));
        }
        if (ctx != null) {
            String chain = ctx.chainNodes().stream()
                .map(n -> n.nodeName() + (n.isDone() ? "[done]" : "[todo]"))
                .collect(Collectors.joining(" -> "));
            System.out.println("  审批链: " + chain + " -> END");
        }
    }

    public boolean hasLogContent() {
        try {
            return Files.exists(Path.of("data/action.log"))
                && Files.size(Path.of("data/action.log")) > 0;
        } catch (IOException e) { return false; }
    }

    /** 退出：live 事件归档到按月文件（迭代器的归档数据源），关闭命令日志。 */
    public void shutdown() {
        try {
            List<AuditEvent> live = rawDispatcher.liveEvents();
            if (!live.isEmpty()) {
                Path dir = Path.of("data/history");
                Files.createDirectories(dir);
                Path f = dir.resolve(YearMonth.now() + ".log");
                try (var w = Files.newBufferedWriter(f,
                        StandardOpenOption.CREATE, StandardOpenOption.APPEND)) {
                    for (AuditEvent e : live) {
                        w.write(e.time() + "|" + e.operator() + "|" + e.action() + "|" + e.ticketId());
                        w.newLine();
                    }
                }
                System.out.println("[归档] " + live.size() + " 条历史事件 -> " + f);
            }
            actionLog.close();
        } catch (IOException e) {
            System.out.println("[归档失败] " + e.getMessage());
        }
    }

    private Ticket requireTicket() {
        if (current == null) throw new IllegalStateException("没有在途工单，先 submit");
        return current;
    }

    private ApprovalContext ctxOf(Ticket t) {
        ApprovalContext ctx = ctxs.get(t.getId());
        if (ctx == null) throw new IllegalStateException("工单无审批现场: " + t.getId());
        return ctx;
    }
}
