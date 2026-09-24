package com.yc.payflow.v1feature;

import java.util.HashMap;
import java.util.Map;

/**
 * 会员策略注册表（对应博客第 04 篇「面向接口」的"注册表"用法）。
 *
 * 【为什么用 Map 而不是 switch(vipLevel)】
 *   注册表把"等级→策略"的映射从代码变成数据，新增等级只需要在
 *   构造时 register，主流程 lookup 一次即可，扩展开放、修改关闭。
 */
public final class VipPolicies {
    private static final Map<Integer, VipPolicy> REGISTRY = new HashMap<>();

    static {
        REGISTRY.put(0, new NormalVipPolicy());
        REGISTRY.put(1, new SilverVipPolicy());
        REGISTRY.put(2, new GoldVipPolicy());
        REGISTRY.put(3, new PlatinumVipPolicy());
    }

    private VipPolicies() {}

    public static VipPolicy of(int level) {
        return REGISTRY.getOrDefault(level, REGISTRY.get(0));
    }
}
