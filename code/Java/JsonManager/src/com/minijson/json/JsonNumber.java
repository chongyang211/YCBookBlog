package com.minijson.json;

/**
 * JSON 数字。同时承载整数和浮点：
 *   • 整数走 longValue（精确 64 位）
 *   • 浮点走 doubleValue（IEEE 754 双精度）
 * isInteger 标记区分两者。
 */
public record JsonNumber(long longValue, double doubleValue, boolean isInteger)
        implements JsonValue {

    public static JsonNumber ofLong(long v) {
        return new JsonNumber(v, v, true);
    }
    public static JsonNumber ofDouble(double v) {
        return new JsonNumber(0L, v, false);
    }

    public int asInt()       { return isInteger ? (int) longValue : (int) doubleValue; }
    public long asLong()     { return isInteger ? longValue : (long) doubleValue; }
    public double asDouble() { return isInteger ? longValue : doubleValue; }

    @Override
    public String toString() {
        return isInteger ? String.valueOf(longValue) : String.valueOf(doubleValue);
    }
}
