package flowkit.action;

import flowkit.domain.Attachment;
import flowkit.domain.TicketStatus;

import java.time.Instant;
import java.util.List;

/**
 * 备忘录 —— 不透明快照：只有工单（Originator）能读。
 * 教学版用 public record 简化（原设计靠包边界保证不透明），
 * 纪律：除 Ticket.snapshot/restore 外，任何代码不得触碰它的内容。
 */
public record TicketMemento(TicketStatus status, List<Attachment> attachments, Instant submittedAt) {
}
