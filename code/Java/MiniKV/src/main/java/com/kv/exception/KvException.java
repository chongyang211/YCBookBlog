package com.kv.exception;

/**
 * KV 异常基类（阶段⑥）。
 *
 * 选 unchecked（继承 RuntimeException）：所有 KV 命令错误对 main 主循环都是
 * "打印错误信息后继续 REPL"，没有真正的恢复动作（Effective Java 第 70 条）。
 *
 * message 给程序员（详细 hint），toReplString() 给用户（Redis 风格统一格式）。
 */
public class KvException extends RuntimeException {

    /** Redis 风格的错误前缀，如 "WRONGTYPE" */
    private final String redisStyleHint;

    public KvException(String redisStyleHint, String message) {
        super(message);
        this.redisStyleHint = redisStyleHint;
    }

    public KvException(String redisStyleHint, String message, Throwable cause) {
        super(message, cause);
        this.redisStyleHint = redisStyleHint;
    }

    public String getRedisStyleHint() { return redisStyleHint; }

    /** REPL 输出格式：(error) <hint> <message> */
    public String toReplString() {
        return "(error) " + redisStyleHint + " " + getMessage();
    }
}
