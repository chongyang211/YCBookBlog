package com.yc.orderforge.v3lspisp;

/**
 * ⚠️ 胖接口受害者标本：一个买家被迫实现 OrderOperations 全部 7 个方法，
 * 其中 5 个与买家无关 —— 只能写空壳/抛异常（05 篇胖 SDK 同款痛点）。
 */
public class FatInterfaceBuyer implements OrderOperations {

    @Override public void placeOrder()  { System.out.println("  [胖接口买家] 下单（唯一真正需要的）"); }
    @Override public void cancelOrder() { System.out.println("  [胖接口买家] 取消订单"); }

    // ↓↓↓ 以下 5 个全是"被迫营业"的空壳 ↓↓↓
    @Override public void refundOrder()            { throw new UnsupportedOperationException("买家不能退款"); }
    @Override public void viewOrder()              { throw new UnsupportedOperationException(); }
    @Override public void modifyPrice()            { throw new UnsupportedOperationException("买家不能改价"); }
    @Override public void adjustStock()            { throw new UnsupportedOperationException(); }
    @Override public void exportFinancialReport()  { throw new UnsupportedOperationException("买家不能导报表"); }
}
