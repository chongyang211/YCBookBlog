package com.minijson.json;

public record JsonBool(boolean value) implements JsonValue {
    public static final JsonBool TRUE  = new JsonBool(true);
    public static final JsonBool FALSE = new JsonBool(false);
    public static JsonBool of(boolean b) { return b ? TRUE : FALSE; }
    @Override public String toString() { return String.valueOf(value); }
}
