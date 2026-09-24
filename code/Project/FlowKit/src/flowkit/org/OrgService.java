package flowkit.org;

import java.util.Map;

/** 组织架构服务桩（生产中对应组织架构中台）。 */
public class OrgService {
    private static final Map<String, String> LEADER_OF = Map.of(
        "yangc", "zhangsan",   // yangc 的直属主管是 zhangsan
        "zhangsan", "lisi",
        "lisi", "wangwu");

    public String getDirectLeader(String userId) {
        String leader = LEADER_OF.get(userId);
        if (leader == null) throw new IllegalStateException("查无此人: " + userId);
        return leader;
    }
}
