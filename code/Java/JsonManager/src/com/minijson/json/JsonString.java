package com.minijson.json;

import java.util.Objects;

public record JsonString(String value) implements JsonValue {
    public JsonString {                                   // 紧凑构造：校验
        Objects.requireNonNull(value, "JsonString.value 不能为 null");
    }
    @Override public String toString() { return "\"" + value + "\""; }
}
