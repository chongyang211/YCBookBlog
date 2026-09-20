package com.kv.store;

public record DoubleValue(double v) implements Value {
    @Override public String typeName() { return "double"; }
    @Override public String toString() { return Double.toString(v); }
}
