package flowkit.history;

import java.io.IOException;
import java.io.UncheckedIOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.time.Instant;
import java.util.Comparator;
import java.util.Iterator;
import java.util.List;
import java.util.stream.Stream;

/**
 * 归档侧迭代器：遍历按月分段的归档文件（data/history/2026-09.log ...）。
 * 行格式：ISO时间|操作人|动作|工单号
 */
public class ArchiveIterator implements Iterator<AuditEvent> {
    private final Iterator<String> lines;
    private AuditEvent next;

    public ArchiveIterator(Path archiveDir) {
        this.lines = readAll(archiveDir);
        advance();
    }

    private static Iterator<String> readAll(Path dir) {
        if (!Files.isDirectory(dir)) return List.<String>of().iterator();
        try (Stream<Path> files = Files.list(dir)) {
            List<String> all = new java.util.ArrayList<>();
            files.sorted(Comparator.comparing(Path::getFileName))
                 .filter(p -> p.getFileName().toString().endsWith(".log"))
                 .forEach(p -> {
                     try { all.addAll(Files.readAllLines(p)); }
                     catch (IOException e) { throw new UncheckedIOException(e); }
                 });
            return all.iterator();
        } catch (IOException e) {
            throw new UncheckedIOException(e);
        }
    }

    private void advance() {
        next = null;
        while (lines.hasNext()) {
            String line = lines.next();
            if (line.isBlank()) continue;
            String[] p = line.split("\\|", 4);
            if (p.length == 4) {
                next = new AuditEvent(Instant.parse(p[0]), p[1], p[2], Long.parseLong(p[3]));
                return;
            }
        }
    }

    @Override public boolean hasNext() { return next != null; }

    @Override public AuditEvent next() {
        AuditEvent e = next;
        advance();
        return e;
    }
}
