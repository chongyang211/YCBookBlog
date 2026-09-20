package com.kv.store;

/**
 * 值类型代数（阶段②）—— sealed interface + 6 个 record。
 *
 * 为什么 sealed 比 enum 好：enum 实例没有自己的字段类型，
 * 而 sealed + record 让每种 Value 携带各自的字段类型（IntValue.v() 直接拿 long，无需 cast）。
 */
public sealed interface Value
        permits NullValue, BoolValue, IntValue, DoubleValue, StringValue, ListValue {

    /** 标识每种类型的字符串名（错误信息中用） */
    String typeName();

    /** 字符串自动类型推断："true" → Bool，"123" → Int，"3.14" → Double，其余 → String */
    static Value parse(String raw) {
        if (raw == null || raw.equalsIgnoreCase("null") || raw.equalsIgnoreCase("nil")) {
            return NullValue.INSTANCE;
        }
        if (raw.equalsIgnoreCase("true"))  return new BoolValue(true);
        if (raw.equalsIgnoreCase("false")) return new BoolValue(false);

        // 整数？
        try { return new IntValue(Long.parseLong(raw)); }
        catch (NumberFormatException ignored) { /* 继续尝试 */ }

        // 小数？
        try { return new DoubleValue(Double.parseDouble(raw)); }
        catch (NumberFormatException ignored) { /* 继续尝试 */ }

        // 默认按字符串
        return new StringValue(raw);
    }
}
