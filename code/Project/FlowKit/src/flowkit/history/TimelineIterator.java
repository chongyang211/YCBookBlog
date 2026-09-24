package flowkit.history;

import java.util.Iterator;

/**
 * 双源归并迭代器 —— 内存事件流 + 归档文件流，各按时间有序，归并成一条平滑时间线。
 * 调用方拿到 Iterable 后 for-each 即可，完全不必知道底下有两种存储结构。
 */
public class TimelineIterator implements Iterator<AuditEvent> {
    private final Iterator<AuditEvent> liveIt;      // 内存中的进行中事件
    private final Iterator<AuditEvent> archiveIt;   // 按月归档文件拼接
    private AuditEvent nextLive;
    private AuditEvent nextArchive;

    public TimelineIterator(Iterator<AuditEvent> liveIt, Iterator<AuditEvent> archiveIt) {
        this.liveIt = liveIt;
        this.archiveIt = archiveIt;
        advanceLive();
        advanceArchive();
    }

    private void advanceLive()    { nextLive    = liveIt.hasNext()    ? liveIt.next()    : null; }
    private void advanceArchive() { nextArchive = archiveIt.hasNext() ? archiveIt.next() : null; }

    @Override public boolean hasNext() { return nextLive != null || nextArchive != null; }

    @Override public AuditEvent next() {
        if (nextArchive == null
            || (nextLive != null && !nextLive.time().isAfter(nextArchive.time()))) {
            AuditEvent e = nextLive; advanceLive(); return e;    // 谁早出谁
        }
        AuditEvent e = nextArchive; advanceArchive(); return e;
    }
}
