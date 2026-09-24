package com.yc.orderforge.v3lspisp;

/**
 * 买家：只实现 BuyerOperations —— 2 个方法，全部有用，0 个空壳。
 * 对照 05 篇：接口代表"能力契约"，能力以最小粒度描述。
 */
public class BuyerUser implements BuyerOperations {

    @Override public void placeOrder()  { System.out.println("  [买家] 下单"); }
    @Override public void cancelOrder() { System.out.println("  [买家] 取消订单"); }
}
