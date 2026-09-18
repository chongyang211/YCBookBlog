package com.yc.orderforge.v1srp;

import com.yc.orderforge.domain.Order;

/**
 * 库存服务（V1 拆出的六件事之一）。
 * SRP：库存变化只有一个驱动者——仓储运营。可独立单测、独立复用。
 */
public class InventoryService {
    private int stockSkuA = 100;

    /** 扣库存（返回 false 表示库存不足） */
    public boolean tryDeduct(Order order) {
        if (stockSkuA < order.totalQty()) return false;
        stockSkuA -= order.totalQty();
        return true;
    }

    /** 【V1 修复点】补偿：支付失败时回滚库存，V0 的"库存凭空消失"就此终结 */
    public void restock(Order order) {
        stockSkuA += order.totalQty();
    }

    public int stock() { return stockSkuA; }
}
