package flowkit.engine;

import flowkit.org.OrgService;

import java.util.HashMap;
import java.util.Map;

/**
 * 策略注册表（简单工厂）—— 策略三要素（定义/创建/使用）里的创建环节。
 * 节点定义里只存策略名，运行时按名取用。
 */
public final class FinderRegistry {
    private static final Map<String, ApproverFinder> REG = new HashMap<>();

    static {
        REG.put("leader",  new LeaderFinder(new OrgService()));
        REG.put("finance", new RoleFinder("FINANCE_APPROVER"));
        REG.put("ladder",  new AmountLadderFinder());
    }

    private FinderRegistry() {}

    public static ApproverFinder of(String name) {
        ApproverFinder f = REG.get(name);
        if (f == null) throw new IllegalArgumentException("未知审批人策略: " + name);
        return f;
    }
}
