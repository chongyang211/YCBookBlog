package com.minijson.mapper;

import com.minijson.annotation.JsonField;
import com.minijson.annotation.JsonIgnore;
import com.minijson.exception.JsonException;
import com.minijson.exception.JsonKeyMissingException;
import com.minijson.json.JsonArray;
import com.minijson.json.JsonBool;
import com.minijson.json.JsonNull;
import com.minijson.json.JsonNumber;
import com.minijson.json.JsonObject;
import com.minijson.json.JsonParser;
import com.minijson.json.JsonString;
import com.minijson.json.JsonValue;
import com.minijson.json.JsonWriter;

import java.lang.reflect.Array;
import java.lang.reflect.Constructor;
import java.lang.reflect.Field;
import java.lang.reflect.Modifier;
import java.lang.reflect.ParameterizedType;
import java.lang.reflect.Type;
import java.time.LocalDate;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.IdentityHashMap;
import java.util.LinkedHashMap;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Map;
import java.util.Queue;
import java.util.Set;

/**
 * 反射映射器：POJO ↔ JsonValue（mini Jackson 雏形）。
 */
public class ObjectMapper {

    // ============ POJO → JsonValue ============

    public JsonValue toJsonValue(Object obj) {
        return toJsonValue(obj, Collections.newSetFromMap(new IdentityHashMap<>()));
    }

    private JsonValue toJsonValue(Object obj, Set<Object> visiting) {
        if (obj == null) return JsonNull.instance();
        if (obj instanceof Boolean b)   return JsonBool.of(b);
        if (obj instanceof Integer i)   return JsonNumber.ofLong(i);
        if (obj instanceof Long l)      return JsonNumber.ofLong(l);
        if (obj instanceof Short s)     return JsonNumber.ofLong(s);
        if (obj instanceof Byte by)     return JsonNumber.ofLong(by);
        if (obj instanceof Float f)     return JsonNumber.ofDouble(f);
        if (obj instanceof Double d)    return JsonNumber.ofDouble(d);
        if (obj instanceof Character c) return new JsonString(String.valueOf(c));
        if (obj instanceof String s)    return new JsonString(s);
        if (obj instanceof Enum<?> e)   return new JsonString(e.name());
        if (obj instanceof LocalDate ld)
            return new JsonString(ld.format(DateTimeFormatter.ISO_DATE));
        if (obj instanceof LocalDateTime ldt)
            return new JsonString(ldt.format(DateTimeFormatter.ISO_DATE_TIME));

        // 数组
        if (obj.getClass().isArray()) return arrayToJson(obj, visiting);

        // 集合
        if (obj instanceof Collection<?> col) {
            List<JsonValue> items = new ArrayList<>(col.size());
            for (Object o : col) items.add(toJsonValue(o, visiting));
            return new JsonArray(items);
        }

        // Map
        if (obj instanceof Map<?, ?> map) {
            LinkedHashMap<String, JsonValue> m = new LinkedHashMap<>();
            for (Map.Entry<?, ?> e : map.entrySet()) {
                m.put(String.valueOf(e.getKey()), toJsonValue(e.getValue(), visiting));
            }
            return new JsonObject(m);
        }

        // 普通对象 —— 反射读字段 + 循环引用检测
        if (!visiting.add(obj)) {
            throw new JsonException("检测到循环引用: " + obj.getClass().getName());
        }
        try {
            return objectToJson(obj, visiting);
        } finally {
            visiting.remove(obj);
        }
    }

    private JsonValue arrayToJson(Object arr, Set<Object> visiting) {
        int len = Array.getLength(arr);
        List<JsonValue> items = new ArrayList<>(len);
        for (int i = 0; i < len; i++) items.add(toJsonValue(Array.get(arr, i), visiting));
        return new JsonArray(items);
    }

    private JsonValue objectToJson(Object obj, Set<Object> visiting) {
        LinkedHashMap<String, JsonValue> map = new LinkedHashMap<>();
        for (Field f : getAllFields(obj.getClass())) {
            if (Modifier.isStatic(f.getModifiers()) || Modifier.isTransient(f.getModifiers())) continue;
            JsonIgnore ignore = f.getAnnotation(JsonIgnore.class);
            if (ignore != null && ignore.value() != JsonIgnore.Direction.DESERIALIZE) continue;

            f.setAccessible(true);
            String key = jsonKey(f);
            try {
                Object value = f.get(obj);
                map.put(key, toJsonValue(value, visiting));
            } catch (IllegalAccessException e) {
                throw new JsonException("反射读字段失败: " + f.getName(), e);
            }
        }
        return new JsonObject(map);
    }

    private static String jsonKey(Field f) {
        JsonField ann = f.getAnnotation(JsonField.class);
        return (ann != null && !ann.name().isEmpty()) ? ann.name() : f.getName();
    }

    private static List<Field> getAllFields(Class<?> clazz) {
        List<Field> out = new ArrayList<>();
        for (Class<?> c = clazz; c != null && c != Object.class; c = c.getSuperclass()) {
            Collections.addAll(out, c.getDeclaredFields());
        }
        return out;
    }

    /** 字符串入口 */
    public String toJson(Object obj)            { return JsonWriter.write(toJsonValue(obj)); }
    public String toJson(Object obj, boolean p) { return JsonWriter.write(toJsonValue(obj), p); }

    // ============ JsonValue → POJO ============

    @SuppressWarnings("unchecked")
    public <T> T fromJson(String src, Class<T> clazz) {
        JsonValue v = JsonParser.parse(src);
        return (T) fromJsonValue(v, clazz, null);
    }

    public <T> T fromJsonValue(JsonValue v, Class<T> clazz) {
        return clazz.cast(fromJsonValue(v, clazz, null));
    }

    /** 核心：根据 targetType 还原 Java 对象 */
    private Object fromJsonValue(JsonValue v, Class<?> rawType, Type genericType) {
        if (v == null || v.isNull()) return null;

        // 基本类型 / 包装类
        if (rawType == String.class)                                 return v.asString();
        if (rawType == boolean.class || rawType == Boolean.class)    return v.asBool();
        if (rawType == int.class     || rawType == Integer.class)    return (int) v.asLong();
        if (rawType == long.class    || rawType == Long.class)       return v.asLong();
        if (rawType == double.class  || rawType == Double.class)     return v.asDouble();
        if (rawType == float.class   || rawType == Float.class)      return (float) v.asDouble();
        if (rawType == short.class   || rawType == Short.class)      return (short) v.asLong();
        if (rawType == byte.class    || rawType == Byte.class)       return (byte) v.asLong();
        if (rawType == char.class    || rawType == Character.class)  return v.asString().charAt(0);
        if (rawType.isEnum())
            return Enum.valueOf((Class<Enum>) rawType.asSubclass(Enum.class), v.asString());
        if (rawType == LocalDate.class)
            return LocalDate.parse(v.asString(), DateTimeFormatter.ISO_DATE);
        if (rawType == LocalDateTime.class)
            return LocalDateTime.parse(v.asString(), DateTimeFormatter.ISO_DATE_TIME);

        // 数组
        if (rawType.isArray()) return jsonToArray(v.asArray(), rawType);

        // 集合（List / Set）
        if (Collection.class.isAssignableFrom(rawType)) {
            return jsonToCollection(v.asArray(), rawType, genericType);
        }

        // Map
        if (Map.class.isAssignableFrom(rawType)) {
            return jsonToMap(v.asObject(), rawType, genericType);
        }

        // 普通对象（递归填字段）
        return jsonToObject(v.asObject(), rawType);
    }

    private Object jsonToArray(JsonArray arr, Class<?> rawType) {
        Class<?> compType = rawType.getComponentType();
        Object out = Array.newInstance(compType, arr.size());
        for (int i = 0; i < arr.size(); i++) {
            Array.set(out, i, fromJsonValue(arr.get(i), compType, null));
        }
        return out;
    }

    @SuppressWarnings({"unchecked", "rawtypes"})
    private Object jsonToCollection(JsonArray arr, Class<?> rawType, Type genericType) {
        Collection col = createCollection(rawType);
        // 类型擦除关键修复点：从 ParameterizedType 拿元素类型
        Class<?> elemClass = Object.class;
        Type elemGeneric = null;
        if (genericType instanceof ParameterizedType pt) {
            Type arg = pt.getActualTypeArguments()[0];
            if (arg instanceof Class<?> cls) elemClass = cls;
            else if (arg instanceof ParameterizedType pt2) {
                elemClass = (Class<?>) pt2.getRawType();
                elemGeneric = pt2;
            }
        }
        for (int i = 0; i < arr.size(); i++) {
            col.add(fromJsonValue(arr.get(i), elemClass, elemGeneric));
        }
        return col;
    }

    @SuppressWarnings({"unchecked", "rawtypes"})
    private Object jsonToMap(JsonObject obj, Class<?> rawType, Type genericType) {
        Map map = createMap(rawType);
        Class<?> valClass = Object.class;
        Type valGeneric = null;
        if (genericType instanceof ParameterizedType pt) {
            Type arg = pt.getActualTypeArguments()[1];
            if (arg instanceof Class<?> cls) valClass = cls;
            else if (arg instanceof ParameterizedType pt2) {
                valClass = (Class<?>) pt2.getRawType();
                valGeneric = pt2;
            }
        }
        for (var e : obj.fields().entrySet()) {
            map.put(e.getKey(), fromJsonValue(e.getValue(), valClass, valGeneric));
        }
        return map;
    }

    @SuppressWarnings("rawtypes")
    private Collection createCollection(Class<?> rawType) {
        if (rawType.isInterface()) {
            if (List.class.isAssignableFrom(rawType))  return new ArrayList<>();
            if (Set.class.isAssignableFrom(rawType))   return new LinkedHashSet<>();
            if (Queue.class.isAssignableFrom(rawType)) return new ArrayDeque<>();
            return new ArrayList<>();
        }
        try { return (Collection) rawType.getDeclaredConstructor().newInstance(); }
        catch (Exception e) { throw new JsonException("创建集合失败: " + rawType, e); }
    }

    @SuppressWarnings("rawtypes")
    private Map createMap(Class<?> rawType) {
        if (rawType.isInterface()) return new LinkedHashMap<>();
        try { return (Map) rawType.getDeclaredConstructor().newInstance(); }
        catch (Exception e) { throw new JsonException("创建 Map 失败: " + rawType, e); }
    }

    private Object jsonToObject(JsonObject src, Class<?> clazz) {
        Object instance;
        try {
            Constructor<?> ctor = clazz.getDeclaredConstructor();
            ctor.setAccessible(true);
            instance = ctor.newInstance();
        } catch (NoSuchMethodException e) {
            throw new JsonException(clazz.getName() + " 必须有无参构造方法", e);
        } catch (Exception e) {
            throw new JsonException("创建实例失败: " + clazz.getName(), e);
        }

        for (Field f : getAllFields(clazz)) {
            if (Modifier.isStatic(f.getModifiers()) || Modifier.isTransient(f.getModifiers())) continue;
            JsonIgnore ignore = f.getAnnotation(JsonIgnore.class);
            if (ignore != null && ignore.value() != JsonIgnore.Direction.SERIALIZE) continue;

            String key = jsonKey(f);
            JsonField ann = f.getAnnotation(JsonField.class);
            if (!src.has(key)) {
                if (ann != null && ann.required()) {
                    throw new JsonKeyMissingException(key);
                }
                continue;       // 不必填且缺失 → 跳过（保持默认值）
            }
            f.setAccessible(true);
            try {
                Object value = fromJsonValue(src.get(key), f.getType(), f.getGenericType());
                f.set(instance, value);
            } catch (IllegalAccessException e) {
                throw new JsonException("反射写字段失败: " + f.getName(), e);
            }
        }
        return instance;
    }
}
