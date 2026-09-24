package flowkit.org;

import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicInteger;

/** 角色服务桩：按角色取人（简单轮询负载均衡）。 */
public class RoleService {
    private static final Map<String, List<String>> ROLE_MEMBERS = Map.of(
        "FINANCE_APPROVER", List.of("lisi", "zhaoqian"),
        "LEGAL",            List.of("lawyer1", "lawyer2"));

    private static final Map<String, AtomicInteger> ROUND_ROBIN = new ConcurrentHashMap<>();

    public static String pickAny(String role) {
        List<String> members = ROLE_MEMBERS.get(role);
        if (members == null) throw new IllegalStateException("角色不存在: " + role);
        int i = ROUND_ROBIN.computeIfAbsent(role, k -> new AtomicInteger())
                           .getAndIncrement() % members.size();
        return members.get(Math.abs(i));
    }
}
