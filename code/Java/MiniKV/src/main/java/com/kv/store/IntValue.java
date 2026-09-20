package com.kv.store;

public record IntValue(long v) implements Value {
    @Override public String typeName() { return "int"; }
    @Override public String toString() { return Long.toString(v); }
}
