package com.yc.payflow.v0shipan;

/**
 * 订单原始输入：一坨"数据袋子"（PO/DTO 风格）。
 * 全 public 字段，无任何行为——是"伪 OOP"的教科书样本。
 *
 * 坏味道检查（对应博客第 08 篇）：
 *   - 数据泥团：这 10 个字段总是一起出现
 *   - 基本类型偏执：city 用 String、carType 用 int、金额用 double
 *   - 没有不变量：字段可以随意被外部改写
 */
public class OrderInput {
    /** 商品单价数组（元） */
    public double[] prices;
    /** 商品数量数组 */
    public int[] counts;
    /** 商品是否是虚拟商品（1 = 是, 0 = 否） */
    public int[] virtualFlags;
    /** 商品是否是跨境商品（1 = 是, 0 = 否） */
    public int[] crossBorderFlags;
    /** 城市："北京" / "上海" / "其他" */
    public String city;
    /** 会员等级：0=普通 1=白银 2=黄金 3=铂金 */
    public int vipLevel;
    /** 优惠券 ID，null 表示无券 */
    public String couponId;
    /** 是否秒杀商品 */
    public boolean isFlashSale;
    /** 是否拼单 */
    public boolean isGroupBuy;
    /** 使用积分数（100 积分 = 1 元） */
    public int usePoints;
    /** 订单编号（仅打印用） */
    public String orderNo;
}
