package flowkit.history;

import java.nio.file.Path;
import java.util.Iterator;
import java.util.List;

/**
 * 审批历史 —— 实现 Iterable，就能用 for-each（语法糖背后就是迭代器）。
 * 两个数据源：内存 live + 按月归档，由 TimelineIterator 归并。
 */
public class TicketHistory implements Iterable<AuditEvent> {
    private final List<AuditEvent> live;        // 内存中的进行中事件
    private final Path archiveDir;              // 归档目录 data/history/

    public TicketHistory(List<AuditEvent> live, Path archiveDir) {
        this.live = live;
        this.archiveDir = archiveDir;
    }

    @Override
    public Iterator<AuditEvent> iterator() {
        return new TimelineIterator(live.iterator(), new ArchiveIterator(archiveDir));
    }
}
