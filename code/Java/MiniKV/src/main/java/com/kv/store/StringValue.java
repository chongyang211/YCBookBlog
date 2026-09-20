package com.kv.store;

public record StringValue(String s) implements Value {
    @Override public String typeName() { return "string"; }
    @Override public String toString() { return "\"" + s + "\""; }
}
