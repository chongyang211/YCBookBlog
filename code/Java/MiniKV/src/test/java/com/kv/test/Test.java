package com.kv.test;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * 自写测试注解（阶段⑪）—— JUnit 5 @Test 的雏形。
 * 同样必须 RUNTIME 保留，TestRunner 反射扫描方法时才读得到。
 */
@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.METHOD)
public @interface Test {
    String description() default "";
}
