package flowkit.engine;

import flowkit.domain.Ticket;
import flowkit.org.RoleService;

/** 策略二：按角色取人（角色池轮询）。 */
public class RoleFinder implements ApproverFinder {
    private final String role;

    public RoleFinder(String role) { this.role = role; }

    @Override public String find(Ticket t) {
        return RoleService.pickAny(role);   // 该角色下任意一人
    }
}
