package com.kv.command;

import com.kv.annotation.Command;
import com.kv.store.Store;

import java.util.Set;

@Command(name = "KEYS", isWrite = false, description = "列出所有 key")
public class KeysCmd extends com.kv.command.Command {
    @Override public String name() { return "KEYS"; }
    @Override public boolean isWrite() { return false; }

    @Override
    public String execute(Store store, String[] tokens) {
        Set<String> ks = store.keys();
        if (ks.isEmpty()) return "(empty)";
        StringBuilder sb = new StringBuilder();
        int i = 1;
        for (String k : ks) sb.append(i++).append(") \"").append(k).append("\"\n");
        return sb.toString().stripTrailing();
    }
}
