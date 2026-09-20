package com.kv.command;

import com.kv.annotation.Command;
import com.kv.store.Store;
import com.kv.store.Value;

@Command(name = "SET", isWrite = true, description = "设置 key 的值")
public class SetCmd extends com.kv.command.Command {
    @Override public String name() { return "SET"; }

    @Override
    public String execute(Store store, String[] tokens) {
        requireArgs(tokens, 3, "SET key value");
        store.set(tokens[1], Value.parse(tokens[2]));
        return "OK";
    }
}
