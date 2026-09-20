package com.kv.command;

import com.kv.annotation.Command;
import com.kv.store.Store;
import com.kv.store.Value;

@Command(name = "GET", isWrite = false, description = "获取 key 的值")
public class GetCmd extends com.kv.command.Command {
    @Override public String name() { return "GET"; }
    @Override public boolean isWrite() { return false; }     // ⭐ 读命令不写 AOF

    @Override
    public String execute(Store store, String[] tokens) {
        requireArgs(tokens, 2, "GET key");
        return store.get(tokens[1])
                .map(Value::toString)
                .orElse("(nil)");
    }
}
