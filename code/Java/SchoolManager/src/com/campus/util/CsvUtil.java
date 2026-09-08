package com.campus.util;

import java.util.ArrayList;
import java.util.List;

/**
 * CSV 工具：RFC 4180 双引号转义。
 */
public class CsvUtil {

    /** 把字段安全地拼成一行 CSV（自动转义逗号 / 引号 / 换行） */
    public static String join(String... fields) {
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < fields.length; i++) {
            if (i > 0) sb.append(',');
            sb.append(escape(fields[i]));
        }
        return sb.toString();
    }

    /** 转义单字段：含 , " \n 时整体加双引号、内部双引号变两个 */
    public static String escape(String field) {
        if (field == null) return "";
        boolean needQuote = field.indexOf(',') >= 0 ||
                            field.indexOf('"') >= 0 ||
                            field.indexOf('\n') >= 0;
        if (!needQuote) return field;
        return '"' + field.replace("\"", "\"\"") + '"';
    }

    /** 反解析一行 CSV（支持 RFC 4180 双引号转义） */
    public static List<String> split(String line) {
        List<String> result = new ArrayList<>();
        StringBuilder cur = new StringBuilder();
        boolean inQuote = false;
        for (int i = 0; i < line.length(); i++) {
            char c = line.charAt(i);
            if (inQuote) {
                if (c == '"') {
                    // 看下一个：是 "" 还是结束 ?
                    if (i + 1 < line.length() && line.charAt(i + 1) == '"') {
                        cur.append('"');
                        i++;
                    } else {
                        inQuote = false;
                    }
                } else {
                    cur.append(c);
                }
            } else {
                if (c == ',') {
                    result.add(cur.toString());
                    cur.setLength(0);
                } else if (c == '"' && cur.length() == 0) {
                    inQuote = true;
                } else {
                    cur.append(c);
                }
            }
        }
        result.add(cur.toString());
        return result;
    }
}
