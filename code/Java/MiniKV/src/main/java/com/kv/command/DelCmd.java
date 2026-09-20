package com.kv.command;

import com.kv.annotation.Command;
import com.kv.store.Store;

@Command(name = "DEL", isWrite = true, description = "删除 key")
public class DelCmd extends com.kv.command.Command {
    @Override public String name() { return "DEL"; }

    @Override
    public String execute(Store store, String[] tokens) {
        requireArgs(tokens, 2, "DEL key");
        return store.del(tokens[1]) ? "(integer) 1" : "(integer) 0";
    }
}
