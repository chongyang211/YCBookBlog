package com.yc.orderforge.v3lspisp;

/**
 * ⚠️ 胖接口标本（05 篇 UserOperations 原案）：
 * 把"所有和订单有关的操作"塞进一个接口——下单/取消/退款/改价/调库存/导报表。
 *
 * 三个角色被强行捆在一起：
 *   买家：只要下单/取消          → 被迫实现 4 个无关方法
 *   客服：要退款/查询            → 被迫实现 4 个无关方法
 *   运营：要改价/调库存/导报表   → 被迫实现 2 个无关方法
 *
 * 未来接口加一个方法 → 所有实现类全部编译失败（05 篇胖 SDK 炸弹同款）。
 * 保留此接口作为反面教材，正式拆分见 BuyerOperations / SupportOperations / AdminOperations。
 */
public interface OrderOperations {
    void placeOrder();
    void cancelOrder();
    void refundOrder();
    void viewOrder();
    void modifyPrice();
    void adjustStock();
    void exportFinancialReport();
}
