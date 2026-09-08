package com.minijson.annotation;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.FIELD)
public @interface JsonIgnore {
    /** 仅序列化时忽略 / 仅反序列化时忽略 / 双向忽略 */
    Direction value() default Direction.BOTH;

    enum Direction { SERIALIZE, DESERIALIZE, BOTH }
}
