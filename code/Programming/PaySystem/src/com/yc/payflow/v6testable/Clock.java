package com.yc.payflow.v6testable;

import java.time.LocalDateTime;
import java.time.LocalTime;

/**
 * ============================================================
 *  Clock 抽象（对应博客第 10 篇「可测试性 · Clock 抽象」）
 *  ------------------------------------------------------------
 *  【为什么必须抽象】：
 *    V0 & V4 里的 FlashSaleStep 直接调 LocalTime.now()——
 *    → 测试秒杀折扣的唯一办法就是"等到晚上 8 点跑"
 *    → 或者 mock 静态方法（PowerMock 等重型工具，脆弱且慢）
 *
 *  【三个特性】：
 *    ① 可观测：now() 是纯函数，返回值一目了然
 *    ② 可控制：测试时注入 FixedClock 想拨到几点拨到几点
 *    ③ 可隔离：生产用 SystemClock 依赖 JVM 时钟，测试用假实现
 * ============================================================
 */
public interface Clock {
    /** 返回当前时间。 */
    LocalDateTime now();

    /** 便捷方法：当前时刻。 */
    default LocalTime nowTime() { return now().toLocalTime(); }

    /** 生产：走 JVM 系统时钟。 */
    static Clock system() { return () -> LocalDateTime.now(); }

    /** 测试：固定在某时刻。 */
    static Clock fixed(LocalDateTime at) { return () -> at; }
}
