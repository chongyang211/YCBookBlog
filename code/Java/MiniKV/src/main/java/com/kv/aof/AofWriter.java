package com.kv.aof;

import java.io.BufferedWriter;
import java.io.IOException;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.nio.channels.FileChannel;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardOpenOption;

/**
 * AOF 追加写引擎（阶段⑦）—— Redis AOF 的教学版。
 *
 * 核心：Append-Only（永远只在文件末尾追加，O(1) 单次写），
 * 启动时按时间顺序重放恢复数据（见 AofReplayer）。
 *
 * write / flush / fsync 三层：
 *   writer.write(line)     → 用户态缓冲区
 *   writer.flush()         → OS 页缓存
 *   channel.force(true)    → 磁盘（真正的持久化）
 */
public class AofWriter implements AutoCloseable {

    private final Path file;
    private final BufferedWriter writer;
    private final FileChannel channel;
    private final FlushPolicy policy;
    private boolean closed = false;                 // close 幂等（正常退出 + ShutdownHook 可能各调一次）

    public AofWriter(Path file, FlushPolicy policy) throws IOException {
        this.file = file;
        this.policy = policy;
        // ⭐ APPEND 模式打开（不存在则创建），永远只在末尾追加
        OutputStream os = Files.newOutputStream(file,
                StandardOpenOption.CREATE,
                StandardOpenOption.APPEND);
        this.writer = new BufferedWriter(new OutputStreamWriter(os, StandardCharsets.UTF_8));
        // 拿到 FileChannel 用于 fsync
        this.channel = FileChannel.open(file, StandardOpenOption.WRITE);
    }

    /** 追加一行命令 */
    public synchronized void append(String line) throws IOException {
        writer.write(line);
        writer.newLine();
        if (policy == FlushPolicy.EVERY_WRITE) {
            writer.flush();
            channel.force(true);              // fsync
        }
    }

    /** 周期性刷盘（由 Main 的 EVERY_SECOND 定时线程调用） */
    public synchronized void flushAndSync() throws IOException {
        writer.flush();
        channel.force(true);
    }

    @Override
    public synchronized void close() throws IOException {
        if (closed) return;
        closed = true;
        try (writer; channel) {
            flushAndSync();                   // 关闭前最后刷一次
        }
    }

    public Path getFile() { return file; }
}
