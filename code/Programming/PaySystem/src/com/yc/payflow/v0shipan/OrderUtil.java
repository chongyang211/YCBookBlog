package com.yc.payflow.v0shipan;

import java.util.Date;

/**
 * ============================================================
 *  V0 屎山版：双 11 雪崩事故代码原景重现
 *  ------------------------------------------------------------
 *  这段代码是博客第 01 篇「面向对象设计思想」开篇双 11 雪崩事故
 *  的浓缩版：1 个函数、多重职责、200+ 行、10+ 个魔法数、3 个
 *  真实 bug 埋在里面，恰如凌晨 1 点告警时看到的样子。
 *
 *  为方便教学，本 V0 精简到 ~250 行（真实事故版是 1284 行），
 *  但保留了 08 篇「坏味道」六大家族的全部代表病灶（共 17 处）。
 *
 *  【读者练习】动手前先做"坏味道扫描赛"（对应 08 篇 top10 表）：
 *    - 你能找出至少 10 处坏味道并叫出名字吗？
 *    - 找到之后，先别看 V1，尝试想想每一处该怎么改
 *
 *  【埋的 bug】（V5 会用特征化测试挖出来）：
 *    1. 满减顺序写反：if (total >= 300) -=50 早于 >= 500 -=100，
 *       导致 500 元订单只减 50、少减 50
 *    2. 会员 GOLD + 满减：GOLD 分支忘了扣满减金额，重复打折
 *    3. 秒杀时段判定用 new Date()：测试只能等到 20:00 才能验证
 * ============================================================
 */
public class OrderUtil {

    /* ---------- 全局状态（坏味道 1：全局可变状态） ---------- */
    /** 全局钱包余额，static——不可测、并发不安全 */
    public static double WALLET_BALANCE = 10000.0;
    /** 全局订单流水号 */
    public static int LAST_ORDER_ID = 0;

    /**
     * 计算订单实付金额并完成扣款、发短信、打印小票。
     *
     * 坏味道 2：上帝方法——一个方法同时干了 6 件事：
     *   ①计算小计 ②叠加营销规则 ③校验优惠券 ④扣钱包余额 ⑤发短信 ⑥打印小票
     *
     * 坏味道 3：长参数列表——直接传 OrderInput 也仅止步于"数据袋子"
     */
    public static double calculate(OrderInput input) {
        // 坏味道 4：注释遮羞——需要用注释说"这一段是算什么"，说明命名/拆分没到位
        // ---------- Step 1: 商品小计 ----------
        double total = 0;
        int skuCount = 0;
        boolean hasCrossBorder = false;
        // 坏味道 5：深嵌套
        for (int i = 0; i < input.prices.length; i++) {
            if (input.prices[i] < 0 || input.counts[i] < 0) {
                // 坏味道 6：吞异常/静默错误
                continue;
            }
            double lineTotal = input.prices[i] * input.counts[i];
            // 坏味道 7：基本类型偏执——virtualFlags[i] == 1 而非 Product 值对象
            if (input.virtualFlags[i] == 1) {
                lineTotal = lineTotal * 0.95; // 虚拟商品打 95 折（魔法数）
            }
            if (input.crossBorderFlags[i] == 1) {
                hasCrossBorder = true;
            }
            total += lineTotal;
            skuCount += input.counts[i];
        }
        // 坏味道 8：魔法数——300、500、1000、50、100、250 全部写死
        // 坏味道 9：Switch 惊悚——阶梯满减用 if-else 树
        // 【BUG 1】顺序写反：>= 300 分支拦截了 >= 500 的订单
        double fullReductionAmount = 0;
        if (total >= 300) {
            fullReductionAmount = 50;
        } else if (total >= 500) {
            fullReductionAmount = 100;
        } else if (total >= 1000) {
            fullReductionAmount = 250;
        }
        total = total - fullReductionAmount;

        // ---------- Step 2: 秒杀限时折扣 ----------
        // 坏味道 10：隐式 IO / 全局时间——测试只能等到 20:00 才能验证
        if (input.isFlashSale) {
            Date now = new Date();
            int hour = now.getHours();
            // 秒杀时段 20:00-22:00 打 8 折
            if (hour >= 20 && hour < 22) {
                total = total * 0.8;
            }
        }

        // ---------- Step 3: 优惠券 ----------
        // 坏味道 11：重复代码——三个 if 分支的减免逻辑几乎一模一样
        if (input.couponId != null) {
            if (input.couponId.startsWith("C10")) {
                // 满 100 减 10
                if (total >= 100) total -= 10;
            } else if (input.couponId.startsWith("C30")) {
                if (total >= 300) total -= 30;
            } else if (input.couponId.startsWith("C50")) {
                if (total >= 500) total -= 50;
            } else if (input.couponId.startsWith("VIP")) {
                // VIP 券叠加会员折扣——业务上其实不允许，但 V0 没校验
                total -= 20;
            }
        }

        // ---------- Step 4: 会员等级折扣 ----------
        // 坏味道 12：Switch 惊悚 + 散弹式修改——vipLevel 判断散落多处
        // 【BUG 2】GOLD 分支的重复打折：
        //   本意是"满减 + 会员 85 折"，但下面这段又减了一次 fullReductionAmount，
        //   500 元订单会被少收 50 元，占毛利 10%
        if (input.vipLevel == 3) {
            // 黄金会员打 85 折
            total = total * 0.85;
            // ↓↓↓ 这一行就是 bug ↓↓↓
            total = total - fullReductionAmount;  // 满减金额被"重复减"了一次
        } else if (input.vipLevel == 2) {
            total = total * 0.9;
        } else if (input.vipLevel == 1) {
            total = total * 0.95;
        }

        // ---------- Step 5: 运费 ----------
        // 坏味道 13：城市硬编码字符串
        double shipping = 0;
        if (total < 99) {
            if ("北京".equals(input.city)) {
                shipping = 12;
            } else if ("上海".equals(input.city)) {
                shipping = 12;
            } else {
                shipping = 15;
            }
        }
        // 生鲜冷链另加 8 元——但 V0 里没有生鲜标记（业务遗漏，坏味道 14：未来性夸夸）
        total += shipping;

        // ---------- Step 6: 跨境税 ----------
        if (hasCrossBorder) {
            total = total + total * 0.119; // 跨境税 11.9%（魔法数）
        }

        // ---------- Step 7: 积分抵扣 ----------
        if (input.usePoints > 0) {
            double deduction = input.usePoints / 100.0;
            // 抵扣不能超过订单总额（否则收负钱）
            if (deduction > total) deduction = total;
            total -= deduction;
        }

        // ---------- Step 8: 最低消费兜底 & 四舍五入 ----------
        if (total < 0.01) total = 0.01;
        total = Math.round(total * 100) / 100.0;

        // ---------- Step 9: 扣款 ----------
        // 坏味道 15：直接改全局变量——不变量无人守，可能扣成负数
        WALLET_BALANCE = WALLET_BALANCE - total;

        // ---------- Step 10: 副作用大杂烩 ----------
        // 坏味道 16：依恋情结——本方法沉迷调用外部对象的字段
        LAST_ORDER_ID++;
        String orderNo = input.orderNo != null ? input.orderNo : "AUTO-" + LAST_ORDER_ID;

        // 坏味道 17：隐式 IO——业务方法内部直接打印 & "发短信"
        System.out.println("[短信] 尊敬的会员，订单 " + orderNo + " 已支付 ¥" + total);
        printReceipt(orderNo, input.city, skuCount, total);

        return total;
    }

    /** 打印小票——本不该属于结算方法（依恋情结的另一面） */
    private static void printReceipt(String orderNo, String city, int skuCount, double total) {
        System.out.println("========== 小票 ==========");
        System.out.println("订单号: " + orderNo);
        System.out.println("城市:   " + city);
        System.out.println("件数:   " + skuCount);
        System.out.println("实付:   ¥" + total);
        System.out.println("余额:   ¥" + WALLET_BALANCE);
        System.out.println("==========================");
    }
}
