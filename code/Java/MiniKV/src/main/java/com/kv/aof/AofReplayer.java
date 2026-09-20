package com.kv.aof;

import com.kv.command.Command;
import com.kv.command.CommandRegistry;
import com.kv.exception.AofCorruptedException;
import com.kv.log.Log;
import com.kv.store.Store;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.stream.Stream;

/**
 * AOF 重放器（阶段⑦）—— 启动时把 AOF 文件逐行重放回 Store。
 *
 * 两种模式：
 *   replayStrict  —— 严格模式：一行损坏就抛异常，整个数据库无法启动（造 BUG #3 现场）
 *   replayLenient —— 宽松模式（生产用）：跳过坏行 + 计数 + 警告，
 *                    半行损坏（kill -9 写到一半）只丢最后一行，其余数据完整恢复
 */
public class AofReplayer {

    /** 重放结果统计 */
    public record Result(int totalLines, int successLines, int skippedLines) {
        @Override public String toString() {
            return String.format("AOF 重放: 共 %d 行, 成功 %d, 跳过 %d",
                    totalLines, successLines, skippedLines);
        }
    }

    /** 严格模式——教学对照：末行半截损坏会让整个启动失败 */
    public static Result replayStrict(Store store, Path aofFile) throws IOException {
        if (!Files.exists(aofFile)) {
            return new Result(0, 0, 0);
        }
        int total = 0, success = 0;
        try (Stream<String> lines = Files.lines(aofFile)) {
            for (String line : (Iterable<String>) lines::iterator) {
                total++;
                if (line.isBlank()) continue;
                String[] tokens = line.split("\\s+");
                Command cmd = CommandRegistry.find(tokens[0])
                        .orElseThrow(() -> new AofCorruptedException("未知命令: " + tokens[0]));
                cmd.execute(store, tokens);
                success++;
            }
        }
        return new Result(total, success, 0);
    }

    /**
     * 宽松模式——生产推荐。
     * 关键改进：① 每行独立 try-catch（一行坏不影响后续）② 明确跳过原因 ③ 统计输出。
     */
    public static Result replayLenient(Store store, Path aofFile) throws IOException {
        if (!Files.exists(aofFile)) return new Result(0, 0, 0);

        int total = 0, success = 0, skipped = 0;
        try (Stream<String> lines = Files.lines(aofFile)) {
            for (String line : (Iterable<String>) lines::iterator) {
                total++;
                if (line.isBlank()) { skipped++; continue; }

                try {
                    String[] tokens = line.split("\\s+");
                    if (tokens.length == 0) { skipped++; continue; }

                    Command cmd = CommandRegistry.find(tokens[0]).orElse(null);
                    if (cmd == null) {
                        Log.warn("[AOF] 第 %d 行: 未知命令 '%s'，跳过", total, tokens[0]);
                        skipped++;
                        continue;
                    }

                    cmd.execute(store, tokens);
                    success++;
                } catch (Exception e) {
                    Log.warn("[AOF] 第 %d 行重放失败 (%s)，跳过: %s",
                            total, e.getClass().getSimpleName(), e.getMessage());
                    skipped++;
                }
            }
        }
        return new Result(total, success, skipped);
    }
}
