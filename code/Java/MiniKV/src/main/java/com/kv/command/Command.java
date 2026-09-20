package com.kv.command;

import com.kv.store.Store;

/**
 * 命令抽象基类（阶段④）—— 模板方法模式。
 * 子类实现 name() / execute()；isWrite() / aofLine() / requireArgs() 是共享默认实现。
 *
 * abstract class 而非 interface：因为有字段无关的共享逻辑（isWrite / aofLine / requireArgs）。
 */
public abstract class Command {

    /** 命令名（大写） */
    public abstract String name();

    /** 执行命令，返回 REPL 回复字符串 */
    public abstract String execute(Store store, String[] tokens);

    /** 是否写命令（决定要不要写 AOF） */
    public boolean isWrite() { return true; }

    /** 拼成 AOF 一行（默认按原 tokens 重新 join） */
    public String aofLine(String[] tokens) {
        return String.join(" ", tokens);
    }

    /** 参数校验工具——不够时抛 usage 错误 */
    protected void requireArgs(String[] tokens, int min, String usage) {
        if (tokens.length < min) {
            throw new IllegalArgumentException("(error) usage: " + usage);
        }
    }
}
