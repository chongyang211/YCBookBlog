package flowkit.engine;

import flowkit.domain.Ticket;

/**
 * 策略模式 —— "怎么找审批人"这个变化的方向抽成接口。
 * 单方法接口，天然函数式：一行 lambda 就是一个策略。
 */
@FunctionalInterface
public interface ApproverFinder {
    String find(Ticket t);
}
