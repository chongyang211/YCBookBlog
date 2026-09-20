package com.kv.store;

/**
 * Value.parse 类型推断演示（阶段② Step 2.3 的临时验证入口）。
 * 运行：java -cp out com.kv.store.ValueDemo
 */
public class ValueDemo {
    public static void main(String[] args) {
        for (String s : new String[]{"true", "123", "3.14", "hello", "null"}) {
            Value v = Value.parse(s);
            System.out.printf("%-8s → %-12s typeName=%s%n", s, v, v.typeName());
        }
    }
}
