package com.yc.orderforge.v4diplod;

import com.yc.orderforge.shared.Money;

import java.util.List;

/**
 * 订单仓储抽象（06 篇多数据库案例的落地）。
 * 高层（对账/履约）只依赖本接口；明天换 MySQL、后天换文件，业务零感知。
 */
public interface OrderRepository {
    void save(String orderNo, Money amount);
    List<String> records();
}
