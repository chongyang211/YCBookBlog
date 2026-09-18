package com.yc.orderforge.v1srp;

import com.yc.orderforge.domain.Order;
import com.yc.orderforge.shared.Money;

/**
 * 通知服务（V1 拆出）。
 *
 * ⚠️ 火车残骸（BUG 3）仍在：四层 getter 链 + 空地址 NPE。
 *    依赖倒置（发短信的 Sender 抽象）与 LOD 修复（getShippingCityName）
 *    是 V4 的活——V1 只把它从上帝类里"搬"出来。
 */
public class NotificationService {

    public void notifyPaid(Order order, Money total) {
        // 坏味道保留：火车残骸（V4 修复）
        String city = order.getCustomer().getAddress().getCity().getName();
        System.out.println("[短信] 尊敬的 " + order.getCustomer().getName()
            + "，订单 " + order.getOrderNo() + " 已支付 " + total + "，配送至 " + city);
    }
}
