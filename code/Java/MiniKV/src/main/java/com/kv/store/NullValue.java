package com.kv.store;

public record NullValue() implements Value {
    public static final NullValue INSTANCE = new NullValue();   // 单例
    @Override public String typeName() { return "null"; }
    @Override public String toString() { return "(nil)"; }
}
