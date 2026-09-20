package com.kv.command;

import com.kv.annotation.Command;
import com.kv.store.Store;

/**
 * 阶段⑤ Step 5.7 的验证命令：加它时零修改 Main / CommandRegistry —— 反射注册自动发现。
 */
@Command(name = "PING", isWrite = false, description = "连通性测试")
public class PingCmd extends com.kv.command.Command {
    @Override public String name() { return "PING"; }
    @Override public boolean isWrite() { return false; }

    @Override
    public String execute(Store store, String[] tokens) {
        return "PONG";
    }
}
