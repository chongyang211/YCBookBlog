package com.minijson.json;

import java.util.Map;

/**
 * JSON 序列化器：compact / pretty 双模式。
 * 注：JDK 17 下用 instanceof 链分发（record 模式匹配 switch 是 JDK 21 特性）。
 */
public final class JsonWriter {

    private JsonWriter() {}

    /** 入口 */
    public static String write(JsonValue v) { return write(v, false); }

    public static String write(JsonValue v, boolean pretty) {
        StringBuilder sb = new StringBuilder(1024);
        writeValue(sb, v, 0, pretty);
        return sb.toString();
    }

    private static void writeValue(StringBuilder sb, JsonValue v, int indent, boolean pretty) {
        if (v instanceof JsonNull)          sb.append("null");
        else if (v instanceof JsonBool b)   sb.append(b.value());
        else if (v instanceof JsonNumber n) sb.append(n.toString());
        else if (v instanceof JsonString s) sb.append('"').append(escape(s.value())).append('"');
        else if (v instanceof JsonArray a)  writeArray(sb, a, indent, pretty);
        else if (v instanceof JsonObject o) writeObject(sb, o, indent, pretty);
    }

    private static void writeArray(StringBuilder sb, JsonArray a, int indent, boolean pretty) {
        if (a.size() == 0) { sb.append("[]"); return; }
        sb.append('[');
        if (pretty) sb.append('\n');
        for (int i = 0; i < a.size(); i++) {
            if (pretty) appendIndent(sb, indent + 1);
            writeValue(sb, a.get(i), indent + 1, pretty);
            if (i < a.size() - 1) sb.append(',');
            if (pretty) sb.append('\n');
        }
        if (pretty) appendIndent(sb, indent);
        sb.append(']');
    }

    private static void writeObject(StringBuilder sb, JsonObject o, int indent, boolean pretty) {
        if (o.fields().isEmpty()) { sb.append("{}"); return; }
        sb.append('{');
        if (pretty) sb.append('\n');
        int i = 0, n = o.fields().size();
        for (Map.Entry<String, JsonValue> e : o.fields().entrySet()) {
            if (pretty) appendIndent(sb, indent + 1);
            sb.append('"').append(escape(e.getKey())).append('"').append(':');
            if (pretty) sb.append(' ');
            writeValue(sb, e.getValue(), indent + 1, pretty);
            if (i < n - 1) sb.append(',');
            if (pretty) sb.append('\n');
            i++;
        }
        if (pretty) appendIndent(sb, indent);
        sb.append('}');
    }

    private static void appendIndent(StringBuilder sb, int level) {
        sb.append("  ".repeat(level));    // 2 空格缩进
    }

    private static String escape(String s) {
        StringBuilder sb = new StringBuilder(s.length() + 8);
        for (int i = 0; i < s.length(); i++) {
            char c = s.charAt(i);
            switch (c) {
                case '"'  -> sb.append("\\\"");
                case '\\' -> sb.append("\\\\");
                case '\b' -> sb.append("\\b");
                case '\f' -> sb.append("\\f");
                case '\n' -> sb.append("\\n");
                case '\r' -> sb.append("\\r");
                case '\t' -> sb.append("\\t");
                default -> {
                    if (c < 0x20) sb.append(String.format("\\u%04x", (int) c));
                    else          sb.append(c);
                }
            }
        }
        return sb.toString();
    }
}
