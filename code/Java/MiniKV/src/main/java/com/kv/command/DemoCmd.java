package com.kv.command;

import com.kv.annotation.Command;
import com.kv.store.Store;
import com.kv.store.StringValue;

/**
 * 批量写入演示数据：DEMO n（用于端到端 round-trip 验证与压测）。
 * 只在 AOF 留一行 "DEMO n"，重放时自动恢复 n 条 SET —— 恰好演示 AOF 的命令重放语义。
 */
@Command(name = "DEMO", isWrite = true, description = "批量写入 n 条演示数据: DEMO n")
public class DemoCmd extends com.kv.command.Command {
    @Override public String name() { return "DEMO"; }

    @Override
    public String execute(Store store, String[] tokens) {
        requireArgs(tokens, 2, "DEMO n");
        int n;
        try {
            n = Integer.parseInt(tokens[1]);
        } catch (NumberFormatException e) {
            return "(error) value is not an integer";
        }
        if (n < 0 || n > 1_000_000) {
            return "(error) n out of range [0, 1000000]";
        }
        for (int i = 1; i <= n; i++) {
            store.set("k" + i, new StringValue("v" + i));
        }
        return "OK (已写入 " + n + " 条)";
    }
}
