package com.yc.orderforge.v3lspisp;

/** 买家角色接口：只要下单/取消（05 篇"角色接口"落地）。 */
public interface BuyerOperations {
    void placeOrder();
    void cancelOrder();
}
