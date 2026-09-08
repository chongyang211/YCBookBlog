package com.minijson.db;

import com.minijson.exception.JsonException;
import com.minijson.json.JsonParser;
import com.minijson.json.JsonValue;
import com.minijson.mapper.ObjectMapper;

import java.lang.reflect.Field;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardOpenOption;
import java.util.ArrayList;
import java.util.Collection;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.Optional;
import java.util.stream.Collectors;

import static java.nio.charset.StandardCharsets.UTF_8;

/**
 * 泛型仓储：持有 T 的 Class 绕过类型擦除 + CRUD + PECS 通配符 + 持久化。
 */
public class Repository<T> {

    private final Class<T> entityClass;
    private final String idFieldName;
    private final Map<String, T> store = new LinkedHashMap<>();
    private final ObjectMapper mapper = new ObjectMapper();

    public Repository(Class<T> entityClass, String idFieldName) {
        this.entityClass = entityClass;
        this.idFieldName = idFieldName;
        try {
            entityClass.getDeclaredField(idFieldName);
        } catch (NoSuchFieldException e) {
            throw new IllegalArgumentException(
                    entityClass.getSimpleName() + " 没有 id 字段: " + idFieldName);
        }
    }

    public Class<T> getEntityClass() { return entityClass; }

    // ===== CRUD =====
    public void save(T entity) {
        Objects.requireNonNull(entity, "entity 不能为 null");
        store.put(extractId(entity), entity);
    }

    public Optional<T> findById(String id) {
        return Optional.ofNullable(store.get(id));
    }

    public List<T> findAll() {
        return new ArrayList<>(store.values());
    }

    public boolean delete(String id) {
        return store.remove(id) != null;
    }

    public int size() { return store.size(); }

    // ===== 通配符 PECS 演示 =====
    public void addAll(Collection<? extends T> source) {
        for (T t : source) save(t);
    }

    public void exportAll(Collection<? super T> destination) {
        destination.addAll(store.values());
    }

    public <E extends T> List<E> findByType(Class<E> sub) {
        return store.values().stream()
                .filter(sub::isInstance)
                .map(sub::cast)
                .collect(Collectors.toList());
    }

    // ===== 持久化 =====
    public void saveToFile(Path path) {
        try {
            String json = mapper.toJson(store.values(), true);
            Files.writeString(path, json, UTF_8,
                    StandardOpenOption.CREATE,
                    StandardOpenOption.TRUNCATE_EXISTING);
        } catch (Exception e) {
            throw new JsonException("保存失败: " + path, e);
        }
    }

    @SuppressWarnings("unchecked")
    public void loadFromFile(Path path) {
        if (!Files.exists(path)) return;
        try {
            String json = Files.readString(path, UTF_8);
            JsonValue v = JsonParser.parse(json);
            for (JsonValue item : v.asArray().items()) {
                T entity = (T) mapper.fromJsonValue(item, entityClass);
                save(entity);
            }
        } catch (Exception e) {
            throw new JsonException("加载失败: " + path, e);
        }
    }

    private String extractId(T entity) {
        try {
            Field f = entityClass.getDeclaredField(idFieldName);
            f.setAccessible(true);
            Object v = f.get(entity);
            if (v == null) throw new JsonException("entity.id 不能为 null");
            return v.toString();
        } catch (Exception e) {
            throw new JsonException("提取 id 失败", e);
        }
    }
}
