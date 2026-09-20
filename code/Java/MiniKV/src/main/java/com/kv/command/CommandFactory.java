package com.kv.command;

import java.util.HashMap;
import java.util.Map;
import java.util.Optional;
import java.util.Set;

/**
 * 阶段④版：手写注册的工厂（教学对照保留）。
 * 痛点：加新命令必须回来改 static 块 —— 该痛点催生了阶段⑤的 CommandRegistry（反射扫包）。
 * 终态请用 CommandRegistry。
 */
public class CommandFactory {

    private static final Map<String, Command> REGISTRY = new HashMap<>();

    static {
        register(new SetCmd());
        register(new GetCmd());
        register(new DelCmd());
        register(new ExpireCmd());
        register(new KeysCmd());
    }

    private static void register(Command cmd) {
        REGISTRY.put(cmd.name().toUpperCase(), cmd);
    }

    public static Optional<Command> find(String name) {
        return Optional.ofNullable(REGISTRY.get(name.toUpperCase()));
    }

    public static Set<String> names() { return REGISTRY.keySet(); }
}
