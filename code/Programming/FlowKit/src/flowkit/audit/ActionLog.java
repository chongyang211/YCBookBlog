package flowkit.audit;

import flowkit.action.AddSignCommand;
import flowkit.action.ApproveCommand;
import flowkit.action.RejectCommand;
import flowkit.action.TaskAction;

import java.io.BufferedWriter;
import java.io.IOException;
import java.io.UncheckedIOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardOpenOption;

/**
 * 命令日志 —— 一行一命令，只追加不改写（对齐 AOF 思想）。
 * 把 describe() 顺序落盘，进程重启后逐条重放即可恢复现场。
 */
public class ActionLog implements AutoCloseable {
    private final Path path;
    private final BufferedWriter writer;

    public ActionLog(Path path) throws IOException {
        this.path = path;
        Files.createDirectories(path.getParent());
        this.writer = Files.newBufferedWriter(path,
            StandardOpenOption.CREATE, StandardOpenOption.APPEND);
    }

    public void write(String line) {
        try {
            writer.write(line);
            writer.newLine();
            writer.flush();                        // 教学版每条 flush；生产版按批次 fsync
        } catch (IOException e) { throw new UncheckedIOException(e); }
    }

    public Path path() { return path; }

    /** 文本 → 命令对象：写日志的逆操作（重放的钥匙）。SUBMIT 行由 Facade 特殊处理（需重建工单）。 */
    public static TaskAction parse(String line) {
        String[] p = line.split("\\s+", 4);
        return switch (p[0]) {
            case "APPROVE" -> new ApproveCommand(p[2]);              // "APPROVE by zhangsan"
            case "ADDSIGN" -> new AddSignCommand(p[1]);              // "ADDSIGN wangwu"
            case "REJECT"  -> new RejectCommand(p[2], reasonOf(line));
            default        -> throw new IllegalStateException("无法识别的日志行: " + line);
        };
    }

    private static String reasonOf(String line) {
        int i = line.indexOf("reason=");
        return i < 0 ? "" : line.substring(i + 7);
    }

    @Override public void close() throws IOException { writer.close(); }
}
