package flowkit.engine;

import flowkit.domain.Ticket;
import flowkit.org.OrgService;

/** 策略一：直属主管。 */
public class LeaderFinder implements ApproverFinder {
    private final OrgService org;

    public LeaderFinder(OrgService org) { this.org = org; }

    @Override public String find(Ticket t) {
        return org.getDirectLeader(t.getApplicant());
    }
}
