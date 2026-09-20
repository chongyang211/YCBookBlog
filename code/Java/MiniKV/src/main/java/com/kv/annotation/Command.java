package com.kv.annotation;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * 命令注解（阶段⑤）—— Spring @Component 扫描的雏形。
 *
 * ⚠️ @Retention 必须是 RUNTIME（造 BUG #2 坑③：默认 CLASS 保留，
 *    编译进 .class 但运行时反射读不到 → 所有命令被静默跳过，且无任何报错）。
 */
@Retention(RetentionPolicy.RUNTIME)         // ⭐ 必须 RUNTIME，反射才能读
@Target(ElementType.TYPE)                    // 只能标在类上
public @interface Command {
    String name();                            // 必填：命令名
    boolean isWrite() default true;          // 默认是写命令
    String description() default "";         // 可选描述
}
