package com.yc.orderforge.v4diplod;

import com.yc.orderforge.shared.Money;

import java.util.ArrayList;
import java.util.List;

/** 内存仓储（演示/单测用）。 */
public class InMemoryOrderRepository implements OrderRepository {

    private final List<String> records = new ArrayList<>();

    @Override
    public void save(String orderNo, Money amount) {
        records.add("订单 " + orderNo + " 实付 " + amount);
    }

    @Override
    public List<String> records() {
        return List.copyOf(records);
    }
}
