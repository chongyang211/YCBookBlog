package com.minijson.json;

public record JsonNull() implements JsonValue {
    private static final JsonNull INSTANCE = new JsonNull();
    public static JsonNull instance() { return INSTANCE; }
    @Override public String toString() { return "null"; }
}
