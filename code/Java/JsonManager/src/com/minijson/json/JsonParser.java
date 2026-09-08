package com.minijson.json;

import com.minijson.exception.JsonParseException;

import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Objects;

/**
 * 递归下降 JSON 解析器：文法规则 ↔ Java 方法，互相调用。
 */
public class JsonParser {

    private final String src;
    private int pos;            // 当前游标
    private int line;           // 当前行号（从 1 开始）
    private int column;         // 当前列号（从 1 开始）

    public JsonParser(String src) {
        this.src = Objects.requireNonNull(src, "JSON 源串不能为 null");
        this.pos = 0;
        this.line = 1;
        this.column = 1;
    }

    /** 入口：解析整个 JSON 字符串 */
    public static JsonValue parse(String src) {
        JsonParser p = new JsonParser(src);
        p.skipWhitespace();
        JsonValue v = p.parseValue();
        p.skipWhitespace();
        if (p.pos != p.src.length()) {
            throw p.error("JSON 末尾还有多余字符: '" + p.peek() + "'");
        }
        return v;
    }

    // ===== 工具方法 =====
    private char peek() {
        if (pos >= src.length()) {
            throw error("JSON 意外结束");
        }
        return src.charAt(pos);
    }

    private char next() {
        char c = peek();
        pos++;
        if (c == '\n') { line++; column = 1; } else { column++; }
        return c;
    }

    private boolean match(char expected) {
        if (pos < src.length() && src.charAt(pos) == expected) {
            next();
            return true;
        }
        return false;
    }

    private void expect(char expected) {
        if (!match(expected)) {
            throw error("期望 '" + expected + "'，实际 '" + (pos < src.length() ? src.charAt(pos) : "EOF") + "'");
        }
    }

    private void skipWhitespace() {
        while (pos < src.length()) {
            char c = src.charAt(pos);
            if (c == ' ' || c == '\t' || c == '\r' || c == '\n') next();
            else break;
        }
    }

    private JsonParseException error(String msg) {
        return new JsonParseException(msg, line, column);
    }

    // ===== 值路由 =====
    private JsonValue parseValue() {
        skipWhitespace();
        char c = peek();
        return switch (c) {
            case '{'                  -> parseObject();
            case '['                  -> parseArray();
            case '"'                  -> parseString();
            case 't', 'f'             -> parseBool();
            case 'n'                  -> parseNull();
            case '-', '0', '1', '2', '3',
                 '4', '5', '6', '7', '8', '9' -> parseNumber();
            default -> throw error("非法 JSON 起始字符: '" + c + "'");
        };
    }

    private JsonValue parseNull() {
        if (src.regionMatches(pos, "null", 0, 4)) {
            for (int i = 0; i < 4; i++) next();
            return JsonNull.instance();
        }
        throw error("期望 'null'");
    }

    private JsonValue parseBool() {
        if (src.regionMatches(pos, "true", 0, 4)) {
            for (int i = 0; i < 4; i++) next();
            return JsonBool.TRUE;
        }
        if (src.regionMatches(pos, "false", 0, 5)) {
            for (int i = 0; i < 5; i++) next();
            return JsonBool.FALSE;
        }
        throw error("期望 'true' 或 'false'");
    }

    // ===== 字符串 =====
    private JsonString parseString() {
        expect('"');
        StringBuilder sb = new StringBuilder();
        while (pos < src.length()) {
            char c = next();
            if (c == '"') {
                return new JsonString(sb.toString());
            }
            if (c == '\\') {
                char esc = next();
                switch (esc) {
                    case '"'  -> sb.append('"');
                    case '\\' -> sb.append('\\');
                    case '/'  -> sb.append('/');
                    case 'b'  -> sb.append('\b');
                    case 'f'  -> sb.append('\f');
                    case 'n'  -> sb.append('\n');
                    case 'r'  -> sb.append('\r');
                    case 't'  -> sb.append('\t');
                    case 'u'  -> sb.append(parseUnicodeEscape());
                    default   -> throw error("非法转义字符: \\" + esc);
                }
            } else {
                sb.append(c);
            }
        }
        throw error("字符串未闭合（缺 '\"'）");
    }

    /** 解析 unicode 转义（反斜杠 u + 4 位 hex） */
    private char parseUnicodeEscape() {
        if (pos + 4 > src.length()) throw error("unicode 转义后不足 4 位 hex");
        StringBuilder hex = new StringBuilder(4);
        for (int i = 0; i < 4; i++) hex.append(next());
        try {
            return (char) Integer.parseInt(hex.toString(), 16);
        } catch (NumberFormatException e) {
            throw error("非法 \\u hex: " + hex);
        }
    }

    // ===== 数字 =====
    private JsonNumber parseNumber() {
        int start = pos;
        if (peek() == '-') next();
        while (pos < src.length() && Character.isDigit(src.charAt(pos))) next();

        boolean isFloat = false;
        if (pos < src.length() && src.charAt(pos) == '.') {
            isFloat = true;
            next();
            while (pos < src.length() && Character.isDigit(src.charAt(pos))) next();
        }
        if (pos < src.length() && (src.charAt(pos) == 'e' || src.charAt(pos) == 'E')) {
            isFloat = true;
            next();
            if (pos < src.length() && (src.charAt(pos) == '+' || src.charAt(pos) == '-')) next();
            while (pos < src.length() && Character.isDigit(src.charAt(pos))) next();
        }

        String text = src.substring(start, pos);
        try {
            return isFloat
                    ? JsonNumber.ofDouble(Double.parseDouble(text))
                    : JsonNumber.ofLong(Long.parseLong(text));
        } catch (NumberFormatException e) {
            throw error("非法数字: " + text);
        }
    }

    // ===== 对象 / 数组（递归） =====
    private JsonObject parseObject() {
        expect('{');
        skipWhitespace();
        LinkedHashMap<String, JsonValue> map = new LinkedHashMap<>();
        if (match('}')) return new JsonObject(map);   // 空对象

        while (true) {
            skipWhitespace();
            JsonString key = parseString();
            skipWhitespace();
            expect(':');
            JsonValue value = parseValue();           // 递归调用
            map.put(key.value(), value);
            skipWhitespace();
            if (match(',')) continue;
            if (match('}')) break;
            throw error("期望 ',' 或 '}'，实际 '" + peek() + "'");
        }
        return new JsonObject(map);
    }

    private JsonArray parseArray() {
        expect('[');
        skipWhitespace();
        List<JsonValue> items = new ArrayList<>();
        if (match(']')) return new JsonArray(items);  // 空数组

        while (true) {
            items.add(parseValue());                  // 递归调用
            skipWhitespace();
            if (match(',')) continue;
            if (match(']')) break;
            throw error("期望 ',' 或 ']'，实际 '" + peek() + "'");
        }
        return new JsonArray(items);
    }
}
