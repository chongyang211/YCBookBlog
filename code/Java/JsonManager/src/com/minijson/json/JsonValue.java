package com.minijson.json;

import com.minijson.exception.JsonTypeException;

/**
 * JSON 6 种类型的封闭代数：JDK 17+ sealed interface。
 * 编译器强制穷尽：新增子类不在 permits 列表中无法编译。
 */
public sealed interface JsonValue
        permits JsonNull, JsonBool, JsonNumber, JsonString, JsonArray, JsonObject {

    /** 是否为 null（JSON 语义，非 Java null） */
    default boolean isNull() { return this instanceof JsonNull; }

    default JsonObject asObject() {
        if (this instanceof JsonObject o) return o;
        throw new JsonTypeException("JsonObject", this.getClass().getSimpleName());
    }
    default JsonArray asArray() {
        if (this instanceof JsonArray a) return a;
        throw new JsonTypeException("JsonArray", this.getClass().getSimpleName());
    }
    default String asString() {
        if (this instanceof JsonString s) return s.value();
        throw new JsonTypeException("JsonString", this.getClass().getSimpleName());
    }
    default long asLong() {
        if (this instanceof JsonNumber n) return n.asLong();
        throw new JsonTypeException("JsonNumber", this.getClass().getSimpleName());
    }
    default double asDouble() {
        if (this instanceof JsonNumber n) return n.asDouble();
        throw new JsonTypeException("JsonNumber", this.getClass().getSimpleName());
    }
    default boolean asBool() {
        if (this instanceof JsonBool b) return b.value();
        throw new JsonTypeException("JsonBool", this.getClass().getSimpleName());
    }
}
