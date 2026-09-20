package com.kv.cli;

import com.kv.aof.AofReplayer;
import com.kv.aof.AofWriter;
import com.kv.aof.FlushPolicy;
import com.kv.command.CommandRegistry;
import com.kv.exception.KvException;
import com.kv.exception.UnknownCommandException;
import com.kv.log.Log;
import com.kv.store.Store;
import com.kv.store.TtlCleaner;

import java.io.IOException;
import java.nio.file.Path;
import java.util.Scanner;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

/**
 * Mini-KV 主入口 —— 11 个阶段的最终形态（v1.0）。
 *
 * 启动流程（对应阶段演进）：
 *   ① CommandRegistry.scanAndRegister —— 阶段⑤ 反射扫包注册命令（Spring 雏形）
 *   ② AofReplayer.replayLenient       —— 阶段⑦ 重放 AOF 恢复数据（宽松模式容忍半行损坏）
 *   ③ AofWriter + EVERY_SECOND 刷盘   —— 阶段⑦ write/flush/fsync 三层策略
 *   ④ TtlCleaner 守护线程             —— 阶段⑩ 惰性 + 主动双删除
 *   ⑤ ShutdownHook 优雅关停           —— 阶段⑩ volatile + interrupt + close
 *   ⑥ REPL 主循环 + KvException 路由  —— 阶段①③⑥
 *
 * 可配参数：
 *   -Dkv.aof.file=appendonly.aof   AOF 文件路径
 *   -Dkv.aof.policy=EVERY_SECOND   刷盘策略（EVERY_WRITE / EVERY_SECOND / NEVER）
 *   -Dlog.level=INFO               日志级别（DEBUG 可看命令注册详情）
 */
public class Main {

    public static void main(String[] args) {
        // ① 注解反射扫描注册命令（阶段⑤）
        CommandRegistry.scanAndRegister("com.kv.command");

        // ② Store + AOF 重放恢复（阶段⑦）
        Store store = new Store();
        Path aofFile = Path.of(System.getProperty("kv.aof.file", "appendonly.aof"));
        FlushPolicy policy = FlushPolicy.valueOf(
                System.getProperty("kv.aof.policy", "EVERY_SECOND").toUpperCase());
        Log.debug("AOF 文件路径: %s", aofFile);

        AofWriter aofWriter;
        try {
            AofReplayer.Result result = AofReplayer.replayLenient(store, aofFile);
            if (result.totalLines() > 0) Log.info("%s", result);
            aofWriter = new AofWriter(aofFile, policy);
        } catch (IOException e) {
            Log.error("AOF 初始化失败: %s", e.getMessage());
            return;
        }

        // ③ EVERY_SECOND 定时刷盘线程（守护线程，阶段⑦ Step 7.2）
        if (policy == FlushPolicy.EVERY_SECOND) {
            ScheduledExecutorService aofScheduler = Executors.newSingleThreadScheduledExecutor(r -> {
                Thread t = new Thread(r, "AOF-Flush");
                t.setDaemon(true);
                return t;
            });
            aofScheduler.scheduleAtFixedRate(() -> {
                try { aofWriter.flushAndSync(); }
                catch (IOException e) { Log.error("[AOF] 刷盘失败: %s", e.getMessage()); }
            }, 1, 1, TimeUnit.SECONDS);
        }

        // ④ TTL 守护线程（阶段⑩）
        TtlCleaner cleaner = new TtlCleaner(store, 1000);
        cleaner.start();

        // ⑤ ShutdownHook：Ctrl+C / 正常退出都会触发（阶段⑩ Step 10.5）
        final AofWriter w = aofWriter;
        Runtime.getRuntime().addShutdownHook(new Thread(() -> {
            Log.info("收到 JVM shutdown 信号...");
            cleaner.shutdownGracefully();
            try {
                w.close();
            } catch (IOException e) {
                Log.error("AOF 关闭失败: %s", e.getMessage());
            }
            Log.info("Mini-KV 已优雅退出");
        }, "ShutdownHook"));

        Log.info("Mini-KV v1.0 启动完成（已注册命令 %d 个，刷盘策略 %s）。输入 quit 退出。",
                CommandRegistry.names().size(), policy);

        // ⑥ REPL 主循环
        try (Scanner sc = new Scanner(System.in)) {
            while (true) {
                System.out.print("kv> ");
                if (!sc.hasNextLine()) break;             // EOF (Ctrl+D)
                String line = sc.nextLine().trim();
                if (line.isEmpty()) continue;
                if (line.equalsIgnoreCase("quit") || line.equalsIgnoreCase("exit")) {
                    System.out.println("bye.");
                    break;
                }

                String[] tokens = line.split("\\s+");
                try {
                    com.kv.command.Command cmd = CommandRegistry.find(tokens[0])
                            .orElseThrow(() -> new UnknownCommandException(tokens[0]));
                    String reply = cmd.execute(store, tokens);
                    System.out.println(reply);

                    // ⭐ 只对成功执行的写命令追加 AOF（读命令跳过）
                    if (cmd.isWrite()) {
                        try {
                            w.append(cmd.aofLine(tokens));
                        } catch (IOException e) {
                            Log.error("[AOF] 写入失败: %s", e.getMessage());
                        }
                    }
                } catch (KvException e) {
                    System.out.println(e.toReplString());            // 统一 Redis 风格
                } catch (IllegalArgumentException e) {
                    System.out.println(e.getMessage());              // requireArgs 的 usage
                } catch (Exception e) {
                    System.out.println("(error) UNEXPECTED " + e.getClass().getSimpleName()
                            + ": " + e.getMessage());                // 兜底
                }
            }
        }
    }
}
