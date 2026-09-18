package com.yc.orderforge.v4diplod;

import com.yc.orderforge.shared.Money;

import java.util.ArrayList;
import java.util.List;

/**
 * 文件仓储（模拟"明天要换存储"的场景）：
 * 换仓储 = 组装时注入本实现，OrderFacade / 对账代码零修改。
 */
public class FileOrderRepository implements OrderRepository {

    private final List<String> buffer = new ArrayList<>();

    @Override
    public void save(String orderNo, Money amount) {
        String line = "[落盘] 订单 " + orderNo + " 实付 " + amount;
        buffer.add(line);
        System.out.println("  " + line);
    }

    @Override
    public List<String> records() {
        return List.copyOf(buffer);
    }
}
