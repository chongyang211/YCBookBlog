package com.kv.command;

import com.kv.annotation.Command;
import com.kv.store.Store;

@Command(name = "EXPIRE", isWrite = true, description = "设置 key 的过期秒数")
public class ExpireCmd extends com.kv.command.Command {
    @Override public String name() { return "EXPIRE"; }

    @Override
    public String execute(Store store, String[] tokens) {
        requireArgs(tokens, 3, "EXPIRE key seconds");
        try {
            long sec = Long.parseLong(tokens[2]);
            return store.expire(tokens[1], sec) ? "(integer) 1" : "(integer) 0";
        } catch (NumberFormatException e) {
            return "(error) value is not an integer";
        }
    }
}
