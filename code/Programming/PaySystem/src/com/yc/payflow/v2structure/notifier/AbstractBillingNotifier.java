package com.yc.payflow.v2structure.notifier;

import java.util.HashSet;
import java.util.Set;

/**
 * ============================================================
 *  AbstractBillingNotifier 通知骨架抽象类（模板方法模式）
 *  ------------------------------------------------------------
 *  【骨架层的核心价值】：把三种通道的"共同流程"沉到基类：
 *    ① 校验接收方是否为空
 *    ② 校验是否在黑名单
 *    ③ 拼接账单文案（common template）
 *    ④ 调用子类实现的 doSend() 真正发送
 *    ⑤ 记录发送日志
 *
 *  子类只需要实现 doSend()——"填空题"而不是"作文题"，这就是模板方法。
 *
 *  ⚠️ 对比：如果只用 interface + default 方法，没法共享 blacklist
 *  这个 Set 字段（接口不能有实例字段），骨架代码就写不下去。
 *  这是 03 篇双层范式无可替代的根本原因。
 * ============================================================
 */
public abstract class AbstractBillingNotifier implements BillingNotifier {

    private final Set<String> blacklist = new HashSet<>();

    /** 模板方法：final 修饰锁住流程，子类只填 doSend */
    @Override
    public final void notify(String recipient, String orderNo, String amount) {
        if (recipient == null || recipient.isEmpty()) {
            throw new IllegalArgumentException("接收方不能为空");
        }
        if (blacklist.contains(recipient)) {
            System.out.println("[" + channelName() + "] 命中黑名单，跳过: " + recipient);
            return;
        }
        String body = renderTemplate(orderNo, amount);
        doSend(recipient, body);
        onSent(recipient, orderNo);
    }

    /** 子类必须实现：真正的发送动作 */
    protected abstract void doSend(String recipient, String body);

    /** 钩子方法：子类可以覆盖发送后回调（默认打日志） */
    protected void onSent(String recipient, String orderNo) {
        System.out.println("[" + channelName() + "] 已发送 " + orderNo + " 至 " + recipient);
    }

    /** 公共文案 —— 三个通道共用 */
    protected String renderTemplate(String orderNo, String amount) {
        return "尊敬的会员，您的订单 " + orderNo + " 已支付 " + amount + "，感谢惠顾。";
    }

    /** 骨架层附带的能力：黑名单管理 */
    public void addBlacklist(String recipient) {
        blacklist.add(recipient);
    }
}
