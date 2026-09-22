package flowkit.domain;

import flowkit.action.TicketMemento;
import flowkit.domain.state.DraftState;
import flowkit.engine.ApprovalContext;

import java.time.Instant;
import java.util.ArrayList;
import java.util.List;

/**
 * 工单 —— 唯一的聚合根。
 * 扮演三个角色：状态模式的 Context（动作全部转发给当前状态对象）、
 * 备忘录的 Originator（snapshot/restore）、建造者的产品（字段全 final，校验收口到 build()）。
 */
public class Ticket {
    private final long id;
    private final String applicant;
    private final String type;
    private final long amount;              // 金额（分）
    private final String reason;
    private TicketState state = new DraftState();
    private Instant submittedAt;
    private final List<Attachment> attachments = new ArrayList<>();

    private Ticket(Builder b) {
        this.id = System.currentTimeMillis();
        this.applicant = b.applicant;
        this.type = b.type;
        this.amount = b.amount;
        this.reason = b.reason;
    }

    // ---------- 状态机委托：Ticket 自己只做一行转发 ----------
    public void submit()                        { state.submit(this); }
    public void approve(ApprovalContext ctx)    { state.approve(this, ctx); }
    public void reject(String reason)           { state.reject(this, reason); }
    public void withdraw()                      { state.withdraw(this); }

    public void setStatus(TicketStatus s)       { this.state = StateFactory.of(s); }
    public TicketStatus getStatus()             { return state.name(); }
    public void touchSubmit()                   { this.submittedAt = Instant.now(); }

    // ---------- 备忘录：Originator ----------
    public TicketMemento snapshot() {
        // 注意深拷贝：浅拷贝会让快照与工单共享同一个 List（§07 的坑）
        return new TicketMemento(getStatus(), new ArrayList<>(attachments), submittedAt);
    }
    public void restore(TicketMemento m) {
        setStatus(m.status());
        attachments.clear();
        attachments.addAll(m.attachments());
        this.submittedAt = m.submittedAt();
    }

    public void addAttachment(Attachment a) { attachments.add(a); }

    public long getId()          { return id; }
    public String getApplicant() { return applicant; }
    public String getType()      { return type; }
    public long getAmount()      { return amount; }
    public String getReason()    { return reason; }
    public Instant getSubmittedAt() { return submittedAt; }
    public List<Attachment> getAttachments() { return List.copyOf(attachments); }

    // ---------- 建造者：多字段 + 分步构造 + 校验收口 ----------
    public static Builder builder() { return new Builder(); }

    public static class Builder {
        private String applicant = "anonymous";
        private String type = "REFUND";
        private long amount;
        private String reason = "";

        public Builder applicant(String v) { this.applicant = v; return this; }
        public Builder type(String v)      { this.type = v; return this; }
        public Builder amount(long v)      { this.amount = v; return this; }
        public Builder reason(String v)    { this.reason = v; return this; }

        public Ticket build() {
            if (amount <= 0) throw new IllegalArgumentException("金额必须大于 0");
            if (applicant == null || applicant.isBlank())
                throw new IllegalArgumentException("申请人不能为空");
            return new Ticket(this);
        }
    }
}
