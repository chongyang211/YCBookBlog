package com.yc.orderforge.metrics;

import java.util.ArrayList;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * 极简代码度量工具（08 篇「圈复杂度体温计」的可运行版）。
 *
 * 实现：基于源码文本的轻量解析——
 *   - 方法定位：类体内（花括号深度 1）匹配方法签名行
 *   - 圈复杂度：方法体内 if/for/while/case/catch/&&/|| 出现次数 + 1
 *
 * 教学工具，不追求精确到编译器级别；对教学工程的规整代码足够准确。
 */
public final class CodeMetrics {

    /** 方法级度量结果 */
    public static final class MethodInfo {
        public final String name;
        public final int loc;
        public final int cyclomatic;
        MethodInfo(String name, int loc, int cc) {
            this.name = name; this.loc = loc; this.cyclomatic = cc;
        }
    }

    private static final Pattern METHOD_DECL = Pattern.compile(
        "^\\s{4}(?:public|private|protected|static|final|synchronized|\\s)+"
        + "[\\w\\<\\>\\[\\],\\.\\s]+\\s(\\w+)\\s*\\([^)]*\\)\\s*(\\{|throws.*)?$");

    private static final Pattern CC_TOKEN = Pattern.compile(
        "\\b(if|for|while|case|catch)\\b|&&|\\|\\|");

    /** 去掉注释（块注释 + 行注释），避免注释里的关键字污染度量 */
    public static String sanitize(String source) {
        return source
            .replaceAll("(?s)/\\*.*?\\*/", "")
            .replaceAll("//.*", "");
    }

    /** 统计一个 Java 源文件里的全部方法度量 */
    public static List<MethodInfo> methods(String rawSource) {
        List<MethodInfo> result = new ArrayList<>();
        String source = sanitize(rawSource);
        String[] lines = source.split("\n");
        int depth = 0;
        String currentName = null;
        int methodStart = -1;
        int cc = 1;

        for (int i = 0; i < lines.length; i++) {
            String line = lines[i];

            if (currentName == null) {
                Matcher m = METHOD_DECL.matcher(line);
                if (m.matches() && depth == 1) {
                    currentName = m.group(1);
                    methodStart = i;
                    cc = 1;
                }
            } else {
                Matcher t = CC_TOKEN.matcher(line);
                while (t.find()) cc++;
            }

            for (char c : line.toCharArray()) {
                if (c == '{') depth++;
                else if (c == '}') {
                    depth--;
                    if (currentName != null && depth == 1) {
                        result.add(new MethodInfo(currentName, i - methodStart + 1, cc));
                        currentName = null;
                    }
                }
            }
        }
        return result;
    }

    /** 文件总行数（非空行，不计注释） */
    public static int loc(String rawSource) {
        int n = 0;
        for (String l : sanitize(rawSource).split("\n")) {
            if (!l.isBlank()) n++;
        }
        return n;
    }

    /** 坏味道扫描：返回命中的坏味道描述列表 */
    public static List<String> smellScan(String fileName, String rawSource) {
        String source = sanitize(rawSource);
        List<String> smells = new ArrayList<>();
        if (loc(source) > 200) {
            smells.add("过大的类（" + fileName + " 全文 " + loc(source) + " 行 > 200）→ 违反 SRP");
        }
        int wreck = countMatches(source,
            "\\w+\\.get\\w+\\(\\)\\.get\\w+\\(\\)\\.get\\w+\\(\\)");
        if (wreck > 0) {
            smells.add("火车残骸（三层以上 getter 链 × " + wreck + " 处）→ 违反 LOD");
        }
        int globals = countMatches(source, "public static (?!final)[^=]+=");
        if (globals > 0) {
            smells.add("全局可变状态（public static 可变字段 × " + globals + " 个）→ 破坏封装");
        }
        int news = countMatches(source, "new \\w+(Service|Sender|Client|Repository)\\(");
        if (news > 0) {
            smells.add("业务代码内 new 具体依赖（× " + news + " 处）→ 违反 DIP");
        }
        int printlns = countMatches(source, "System\\.out\\.println");
        if (printlns > 0) {
            smells.add("业务方法隐式 IO（System.out × " + printlns + " 处）→ 阻碍变化");
        }
        for (MethodInfo m : methods(source)) {
            if (m.cyclomatic >= 10) {
                smells.add("方法圈复杂度超标（" + m.name + " CC=" + m.cyclomatic + " >= 10）");
            }
            if (m.loc >= 40) {
                smells.add("过长的方法（" + m.name + " " + m.loc + " 行 >= 40）→ 违反 SRP");
            }
        }
        return smells;
    }

    private static int countMatches(String s, String regex) {
        Matcher m = Pattern.compile(regex).matcher(s);
        int n = 0;
        while (m.find()) n++;
        return n;
    }

    private CodeMetrics() {}
}
