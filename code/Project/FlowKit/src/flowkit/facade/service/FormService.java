package flowkit.facade.service;

import flowkit.domain.Ticket;

/** 表单服务桩：审批中锁定表单，撤回/驳回解锁。 */
public class FormService {
    public void lock(Ticket t)   { System.out.println("[表单] 工单#" + t.getId() + " 已锁定（审批中不可编辑）"); }
    public void unlock(Ticket t) { System.out.println("[表单] 工单#" + t.getId() + " 已解锁（可编辑）"); }
}
