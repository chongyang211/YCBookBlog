package flowkit.domain;

import flowkit.domain.state.ApprovedState;
import flowkit.domain.state.DraftState;
import flowkit.domain.state.RejectedState;
import flowkit.domain.state.ReviewingState;
import flowkit.domain.state.WithdrawnState;

import java.util.Map;

/**
 * 状态对象注册表 —— 状态类无状态（不含工单数据），可全部复用（享元思想的雏形）。
 */
public final class StateFactory {
    private static final Map<TicketStatus, TicketState> STATES = Map.of(
        TicketStatus.DRAFT,     new DraftState(),
        TicketStatus.REVIEWING, new ReviewingState(),
        TicketStatus.REJECTED,  new RejectedState(),
        TicketStatus.WITHDRAWN, new WithdrawnState(),
        TicketStatus.APPROVED,  new ApprovedState());

    private StateFactory() {}

    public static TicketState of(TicketStatus s) {
        TicketState st = STATES.get(s);
        if (st == null) throw new IllegalArgumentException("未知状态: " + s);
        return st;
    }
}
