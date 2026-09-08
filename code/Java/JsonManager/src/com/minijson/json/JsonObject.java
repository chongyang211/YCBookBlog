package com.minijson.json;

import java.util.Collections;
import java.util.LinkedHashMap;
import java.util.Map;
import java.util.Set;

public record JsonObject(Map<String, JsonValue> fields) implements JsonValue {

    public JsonObject {                                   // 紧凑构造
        // 防御性拷贝 + 保留插入顺序
        fields = Collections.unmodifiableMap(new LinkedHashMap<>(fields));
    }

    public static JsonObject empty() { return new JsonObject(new LinkedHashMap<>()); }

    /** 流式构建器（链式 add 风格） */
    public static Builder builder() { return new Builder(); }

    public static class Builder {
        private final LinkedHashMap<String, JsonValue> map = new LinkedHashMap<>();
        public Builder add(String key, JsonValue v) { map.put(key, v); return this; }
        public Builder add(String key, String v)   { return add(key, new JsonString(v)); }
        public Builder add(String key, long v)     { return add(key, JsonNumber.ofLong(v)); }
        public Builder add(String key, double v)   { return add(key, JsonNumber.ofDouble(v)); }
        public Builder add(String key, boolean v)  { return add(key, JsonBool.of(v)); }
        public JsonObject build()                  { return new JsonObject(map); }
    }

    public boolean has(String key)   { return fields.containsKey(key); }
    public JsonValue get(String key) { return fields.get(key); }
    public Set<String> keys()        { return fields.keySet(); }
}
