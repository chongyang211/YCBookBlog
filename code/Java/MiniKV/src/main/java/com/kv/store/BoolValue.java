package com.kv.store;

public record BoolValue(boolean v) implements Value {
    @Override public String typeName() { return "bool"; }
    @Override public String toString() { return v ? "true" : "false"; }
}
