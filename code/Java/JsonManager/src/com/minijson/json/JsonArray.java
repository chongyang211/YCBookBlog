package com.minijson.json;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public record JsonArray(List<JsonValue> items) implements JsonValue {
    public JsonArray {                                    // 紧凑构造
        items = List.copyOf(items);                       // 防御性拷贝 + 不可变
    }
    public static JsonArray empty() { return new JsonArray(Collections.emptyList()); }
    public static JsonArray of(JsonValue... vs) {
        return new JsonArray(new ArrayList<>(List.of(vs)));
    }
    public int size() { return items.size(); }
    public JsonValue get(int i) { return items.get(i); }
}
