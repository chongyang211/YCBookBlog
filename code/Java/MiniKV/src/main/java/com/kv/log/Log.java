package com.kv.log;

import java.io.PrintStream;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;

/**
 * 自写日志门面（阶段⑧）—— SLF4J 的极简版。
 *
 * 设计要点：
 *   1. final class + 私有构造：禁止实例化（Effective Java 第 4 条）
 *   2. volatile Level current：运行时改级别需要可见性
 *   3. WARN/ERROR 走 System.err（IDE 红色显示）
 *   4. 级别不够直接 return，String.format 无开销
 *
 * 级别切换：java -Dlog.level=DEBUG -cp out com.kv.cli.Main
 */
public final class Log {

    public enum Level {
        DEBUG(0), INFO(1), WARN(2), ERROR(3);
        final int order;
        Level(int o) { this.order = o; }
    }

    private static final DateTimeFormatter FMT = DateTimeFormatter.ofPattern("HH:mm:ss.SSS");
    private static volatile Level current = parseLevelFromSystem();

    private Log() {}

    private static Level parseLevelFromSystem() {
        String prop = System.getProperty("log.level", "INFO");
        try { return Level.valueOf(prop.toUpperCase()); }
        catch (IllegalArgumentException e) { return Level.INFO; }
    }

    public static void setLevel(Level lv) { current = lv; }

    public static void debug(String msg, Object... args) { log(Level.DEBUG, System.out, msg, args); }
    public static void info (String msg, Object... args) { log(Level.INFO,  System.out, msg, args); }
    public static void warn (String msg, Object... args) { log(Level.WARN,  System.err, msg, args); }
    public static void error(String msg, Object... args) { log(Level.ERROR, System.err, msg, args); }

    private static void log(Level lv, PrintStream out, String msg, Object... args) {
        if (lv.order < current.order) return;       // 级别过滤
        String formatted = (args.length == 0) ? msg : String.format(msg, args);
        out.printf("%s [%s] %s%n", LocalDateTime.now().format(FMT), lv, formatted);
    }
}
