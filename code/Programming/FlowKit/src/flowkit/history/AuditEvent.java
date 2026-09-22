package flowkit.history;

import java.time.Instant;

/** 统一的历史事件（跨内存与归档两种存储结构）。 */
public record AuditEvent(Instant time, String operator, String action, long ticketId) {
}
