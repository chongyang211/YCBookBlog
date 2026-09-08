package com.minijson.db;

import java.nio.file.Files;
import java.nio.file.Path;
import java.util.LinkedHashMap;
import java.util.Map;
import java.util.NoSuchElementException;
import java.util.Set;

/**
 * 内存数据库：动态注册多表 + 启动加载 + 关闭保存。
 */
public class MiniDB {

    private final Path baseDir;
    private final Map<String, Repository<?>> tables = new LinkedHashMap<>();

    public MiniDB(Path baseDir) throws Exception {
        this.baseDir = baseDir;
        Files.createDirectories(baseDir);
    }

    /** 动态注册表 */
    public <T> Repository<T> register(String name, Class<T> entityClass, String idField) {
        if (tables.containsKey(name)) {
            throw new IllegalStateException("表已存在: " + name);
        }
        Repository<T> repo = new Repository<>(entityClass, idField);
        tables.put(name, repo);
        return repo;
    }

    @SuppressWarnings("unchecked")
    public <T> Repository<T> table(String name) {
        Repository<?> r = tables.get(name);
        if (r == null) throw new NoSuchElementException("表不存在: " + name);
        return (Repository<T>) r;
    }

    public Set<String> tableNames() { return tables.keySet(); }

    /** 启动加载所有表 */
    public void loadAll() {
        for (var e : tables.entrySet()) {
            Path p = baseDir.resolve(e.getKey() + ".json");
            e.getValue().loadFromFile(p);
        }
    }

    /** 关闭保存所有表 */
    public void saveAll() {
        for (var e : tables.entrySet()) {
            Path p = baseDir.resolve(e.getKey() + ".json");
            e.getValue().saveToFile(p);
        }
    }
}
