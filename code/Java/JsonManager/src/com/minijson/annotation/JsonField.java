package com.minijson.annotation;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

@Retention(RetentionPolicy.RUNTIME)        // 必须 RUNTIME，反射才能读
@Target(ElementType.FIELD)                  // 仅限字段
public @interface JsonField {

    /** JSON key 名（默认与字段名同名） */
    String name() default "";

    /** 是否必填，反序列化时 JSON 缺该 key 抛 JsonKeyMissingException */
    boolean required() default false;

    /** 自定义格式（本案例仅日期支持） */
    String format() default "";
}
