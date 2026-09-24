package flowkit.task;

import flowkit.stats.TaskVisitor;

import java.util.List;

/**
 * 组合模式 —— 单任务与任务组共用接口。
 * "（A 会签 B）或（C 会签 D）"这类嵌套分组，靠它统一处理。
 */
public interface TaskComponent {
    boolean isApproved();
    List<TaskComponent> pending();          // 未完成的子任务
    String pretty(int indent);

    /** 访问者双分派第一步：我告诉你我是谁 */
    void accept(TaskVisitor v);
}
