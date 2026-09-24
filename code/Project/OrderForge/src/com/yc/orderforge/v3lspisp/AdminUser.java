package com.yc.orderforge.v3lspisp;

/**
 * 运营：按需组合多个角色接口（实现 BuyerOperations + AdminOperations）。
 * ISP 的"组合"手段（05 篇 §5.3）：类按需实现，谁也不绑架谁。
 */
public class AdminUser implements BuyerOperations, AdminOperations {

    @Override public void placeOrder()  { System.out.println("  [运营] 下单"); }
    @Override public void cancelOrder() { System.out.println("  [运营] 取消订单"); }
    @Override public void modifyPrice() { System.out.println("  [运营] 改价"); }
    @Override public void adjustStock() { System.out.println("  [运营] 调库存"); }
    @Override public void exportFinancialReport() { System.out.println("  [运营] 导出财务报表"); }
}
