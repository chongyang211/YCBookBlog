package flowkit.engine;

import flowkit.domain.Ticket;

/** 抄送装饰器：进入节点时通知抄送人（又一个"加料"维度，可与超时装饰器叠加）。 */
public class CcDecorator extends NodeDecorator {
    private final String ccTo;

    public CcDecorator(NodeDef inner, String ccTo) {
        super(inner);
        this.ccTo = ccTo;
    }

    public CcDecorator(NodeDef inner) { this(inner, "主管"); }

    @Override public void enter(Ticket t, ApprovalContext ctx) {
        super.enter(t, ctx);                            // 原活照干
        System.out.println("[抄送] 已抄送 " + ccTo + ": 工单#" + t.getId());   // 加料
    }
}
