package flowkit.domain;

/**
 * 工单状态枚举 —— §03 止血层：让 "APPROED" 这类 typo 从"运行期静默 bug"变成"编译期错误"。
 */
public enum TicketStatus {
    DRAFT,          // 草稿
    REVIEWING,      // 审批中
    REJECTED,       // 已驳回
    WITHDRAWN,      // 已撤回
    APPROVED        // 已通过
}
