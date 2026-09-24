package com.yc.orderforge.v0god;

import com.yc.orderforge.domain.Order;
import com.yc.orderforge.shared.Money;

/**
 * ============================================================
 *  V0 上帝类：OrderManager
 *  ------------------------------------------------------------
 *  「常见设计原则」专栏综合案例的起点。模拟一个订单履约链路：
 *  下单 → 扣库存 → 计价 → 支付 → 运费 → 通知 → 对账。
 *
 *  每一周的需求单看都"合理"，叠起来就成了灾难（02 篇详情页上帝类同款）：
 *    第 1 周：只有"下单+扣库存"            → 50 行，没毛病
 *    第 2 周：运营要促销折扣               → 加 if-else
 *    第 3 周：接了支付                     → 加支付分支
 *    第 4 周：物流要收运费                 → 加运费分支
 *    第 5 周：要发短信通知 + 财务要对账     → 塞进同一个方法
 *
 *  【埋的 3 个真实 bug】（度量会话挖出，后续手术逐一修复）：
 *    BUG 1【计价】会员折扣先于满减执行 → 1000 元 GOLD 订单
 *              多收 125 元（03 篇"顺序写反，全量用户多扣钱"）
 *    BUG 2【支付】信用卡单笔 5000 限制在扣库存之后才抛异常，
 *              库存已扣、订单失败、库存凭空消失
 *    BUG 3【通知】火车残骸 order.getCustomer().getAddress()
 *              .getCity().getName() 遇到空地址直接 NPE，
 *              支付成功、通知崩溃、订单卡死（07 篇 31 文件事故同款）
 * ============================================================
 */
public class OrderManager {

    /* ---------- 全局可变状态（坏味道 1：全局状态，不可测 + 并发不安全） ---------- */
    public static int STOCK_SKU_A = 100;                       // 坏味道 1
    public static Money WALLET = Money.ofYuan(10000);           // 坏味道 1
    public static int ORDER_SEQ = 0;                            // 坏味道 1
    public static final java.util.List<String> AUDIT_LOG =      // 坏味道 1
        new java.util.ArrayList<>();

    /**
     * 下单主流程——上帝方法（坏味道 2）：一个方法干 6 件事：
     * ①校验 ②扣库存 ③计价 ④运费 ⑤支付 ⑥通知+对账。
     *
     * 圈复杂度 16（metrics 会话现场度量，>10 警戒线）。
     */
    public String placeOrder(Order order) {
        // ---------- ① 校验（坏味道 3：静默吞错） ----------
        if (order == null || order.getLines().isEmpty()) {
            throw new IllegalArgumentException("订单不能为空");
        }
        for (Order.OrderLine l : order.getLines()) {
            if (l.getQty() <= 0) {
                continue;                                       // 坏味道 3
            }
        }

        // ---------- ② 扣库存 ----------
        STOCK_SKU_A -= order.totalQty();                        // 坏味道 4：无校验直接扣

        // ---------- ③ 计价：if-else 森林（坏味道 5：Switch 惊悚） ----------
        Money total = order.subtotal();

        // 【BUG 1】顺序写反：会员折扣先于满减执行。
        //   业务规则：满减优先，会员折扣作用在满减后的金额上。
        //   1000 元 GOLD 订单应得 (1000-250)*0.9 = ¥675，
        //   V0 实得 1000*0.9=900 → 只命中"满500减100" → ¥800，多收 ¥125。
        if (order.isVip()) {                                    // 坏味道 5
            total = total.multiply(0.9);                        // 坏味道 6：魔法数
        }
        if (total.greaterOrEqual(Money.ofYuan(1000))) {
            total = total.minus(Money.ofYuan(250));
        } else if (total.greaterOrEqual(Money.ofYuan(500))) {
            total = total.minus(Money.ofYuan(100));
        } else if (total.greaterOrEqual(Money.ofYuan(300))) {
            total = total.minus(Money.ofYuan(50));
        }
        if (order.getCoupon() != null) {                        // 坏味道 5
            total = total.minus(order.getCoupon());
        }
        if (order.isNewUser()) {
            total = total.minus(Money.ofYuan(30));
        }

        // ---------- ④ 运费 ----------
        if (!order.subtotal().greaterOrEqual(Money.ofYuan(99))) {
            total = total.plus(Money.ofYuan(12));
        }

        // ---------- ⑤ 支付（坏味道 7：渠道分支硬编码） ----------
        // 【BUG 2】信用卡单笔 5000 限制在这里才检查——
        //   而库存已经在 ② 扣掉了。异常一抛，库存凭空消失，无人回滚。
        if (order.getPayType() == 1) {
            if (total.greaterThan(Money.ofYuan(5000))) {
                throw new IllegalStateException("信用卡单笔限额 5000，支付失败");
            }
        }
        WALLET = WALLET.minus(total);                           // 坏味道 8：直改全局

        // ---------- ⑥ 通知 + 对账（坏味道 9：副作用大杂烩） ----------
        ORDER_SEQ++;
        String orderNo = order.getOrderNo() != null
            ? order.getOrderNo() : "AUTO-" + ORDER_SEQ;

        // 【BUG 3】火车残骸（坏味道 10，07 篇同款）：
        //   穿透 Customer→Address→City 四层内部结构。
        //   游客/新用户没有地址 → getAddress() 返回 null → NPE。
        //   而此时④已扣款成功：钱扣了、通知崩了、订单卡死。
        String city = order.getCustomer()
            .getAddress()                                       // 坏味道 10
            .getCity()
            .getName();

        System.out.println("[短信] 尊敬的 " + order.getCustomer().getName()
            + "，订单 " + orderNo + " 已支付 " + total + "，配送至 " + city);
        AUDIT_LOG.add("订单 " + orderNo + " 实付 " + total);      // 坏味道 11：隐式全局写

        return orderNo;
    }

    /** 打印小票——本不属于 OrderManager 的职责（坏味道 12：依恋情结）。 */
    public static void printReceipt(Order order, Money total) {
        System.out.println("========== 小票 ==========");
        System.out.println("订单号: " + order.getOrderNo());
        System.out.println("实付:   " + total);
        System.out.println("库存:   " + STOCK_SKU_A);
        System.out.println("钱包:   " + WALLET);
        System.out.println("==========================");
    }
}
