package com.yc.orderforge.v4diplod;

import com.yc.orderforge.domain.Order;
import com.yc.orderforge.shared.Money;

/**
 * 通知服务（V4 DIP+LOD 双手术版）。
 *
 * DIP 手术：构造注入 MessageSender —— 发什么渠道由组装决定，业务不再写死。
 * LOD 手术：城市改走 order.getShippingCityName()（直接朋友暴露业务方法，
 *           空地址返回"未知城市"）—— 四层火车残骸与 NPE（BUG 3）就此消灭。
 */
public class NotificationService {

    private final MessageSender sender;

    public NotificationService(MessageSender sender) {
        this.sender = sender;
    }

    public void notifyPaid(Order order, Money total) {
        String city = order.getShippingCityName();   // ← 旧: getCustomer().getAddress()...（4 层 + NPE）
        sender.send(order.getCustomer().getPhone(),
            "尊敬的 " + order.getCustomer().getName() + "，订单 "
                + order.getOrderNo() + " 已支付 " + total + "，配送至 " + city);
    }
}
