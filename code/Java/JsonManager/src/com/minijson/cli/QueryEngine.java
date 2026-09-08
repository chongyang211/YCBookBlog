package com.minijson.cli;

import com.minijson.db.MiniDB;
import com.minijson.db.Repository;
import com.minijson.exception.JsonException;

import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.List;
import java.util.function.Predicate;

/**
 * SELECT WHERE 简易查询引擎（反射读字段比较）。
 */
public class QueryEngine {

    private final MiniDB db;

    public QueryEngine(MiniDB db) { this.db = db; }

    public List<?> execute(String cmd) {
        String[] parts = cmd.trim().split("\\s+");
        if (parts.length < 2 || !parts[0].equalsIgnoreCase("SELECT")) {
            throw new IllegalArgumentException("仅支持: SELECT <table> [WHERE <field> <op> <value>]");
        }
        String tableName = parts[1];
        Repository<?> repo = db.table(tableName);

        if (parts.length == 2) return repo.findAll();    // 全表

        if (parts.length != 6 || !parts[2].equalsIgnoreCase("WHERE")) {
            throw new IllegalArgumentException("WHERE 子句格式错误");
        }
        String fieldName = parts[3];
        String op        = parts[4];
        String literal   = parts[5];

        Field f;
        try {
            f = repo.getEntityClass().getDeclaredField(fieldName);
            f.setAccessible(true);
        } catch (NoSuchFieldException e) {
            throw new JsonException(repo.getEntityClass().getSimpleName() + " 没有字段: " + fieldName);
        }

        Predicate<Object> pred = makePredicate(f, op, literal);
        List<Object> out = new ArrayList<>();
        for (Object e : repo.findAll()) {
            try {
                if (pred.test(f.get(e))) out.add(e);
            } catch (IllegalAccessException ex) {
                throw new JsonException("反射读字段失败: " + fieldName);
            }
        }
        return out;
    }

    private Predicate<Object> makePredicate(Field f, String op, String literal) {
        Class<?> t = f.getType();
        // 数字字段
        if (t == int.class || t == Integer.class || t == long.class || t == Long.class) {
            long target = Long.parseLong(literal);
            return v -> {
                long cur = ((Number) v).longValue();
                return switch (op) {
                    case "="  -> cur == target;
                    case "!=" -> cur != target;
                    case ">"  -> cur > target;
                    case ">=" -> cur >= target;
                    case "<"  -> cur < target;
                    case "<=" -> cur <= target;
                    default -> throw new IllegalArgumentException("数字字段不支持 op: " + op);
                };
            };
        }
        // 字符串字段（去掉两端引号）
        String target = literal.replaceAll("^[\"']|[\"']$", "");
        return v -> {
            String cur = String.valueOf(v);
            return switch (op) {
                case "="  -> cur.equals(target);
                case "!=" -> !cur.equals(target);
                case "~"  -> cur.contains(target);
                default -> throw new IllegalArgumentException("字符串字段不支持 op: " + op);
            };
        };
    }
}
