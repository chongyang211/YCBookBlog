package com.kv.command;

import com.kv.annotation.Command;
import com.kv.exception.KvCommandRegisterException;
import com.kv.log.Log;

import java.lang.reflect.Constructor;
import java.net.URL;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.HashMap;
import java.util.Map;
import java.util.Optional;
import java.util.Set;
import java.util.stream.Stream;

/**
 * 命令注册中心（阶段⑤）—— 迷你版 Spring @Component 扫描。
 *
 * 反射六件套（背下来）：
 *   1. Class.forName(fqcn)             ← 强制加载类
 *   2. clazz.isAnnotationPresent(...)  ← 过滤注解
 *   3. clazz.getAnnotation(...)        ← 读注解参数
 *   4. clazz.getDeclaredConstructor()  ← 拿到无参构造（包含私有）
 *   5. ctor.setAccessible(true) + newInstance()  ← 反射 new 实例
 *   6. CACHE.put(name, instance)       ← 缓存到 Map
 *
 * 性能铁律：启动时反射一次，运行时走 HashMap.get——Spring / Hibernate / Jackson 都这套路。
 * 加新命令 = 新建一个类标 @Command，零修改 Main / Registry。
 */
public class CommandRegistry {

    // 注意：本文件同时用到注解 com.kv.annotation.Command 和抽象基类 com.kv.command.Command（同名），
    // 注解走 import，抽象基类用全限定名区分（教学文档 §7.2 的经典同名处理）。
    private static final Map<String, com.kv.command.Command> CACHE = new HashMap<>();

    /** 启动入口：扫描指定包下所有标 @Command 的类 */
    public static void scanAndRegister(String packageName) {
        try {
            // 1. 把 com.kv.command → com/kv/command
            String path = packageName.replace('.', '/');
            URL resource = Thread.currentThread().getContextClassLoader().getResource(path);
            if (resource == null) {
                throw new KvCommandRegisterException("找不到包目录: " + packageName);
            }

            Path dir = Paths.get(resource.toURI());

            // 2. 列出目录下所有 .class
            try (Stream<Path> stream = Files.list(dir)) {
                stream.filter(p -> p.toString().endsWith(".class"))
                      .forEach(p -> tryRegister(p, packageName));
            }
        } catch (KvCommandRegisterException e) {
            throw e;
        } catch (Exception e) {
            throw new KvCommandRegisterException("扫描包失败: " + packageName, e);
        }
    }

    private static void tryRegister(Path classFile, String packageName) {
        String fileName = classFile.getFileName().toString();
        String simpleName = fileName.substring(0, fileName.length() - ".class".length());
        String fqcn = packageName + "." + simpleName;

        Class<?> clazz;
        try {
            clazz = Class.forName(fqcn);            // 3. 强制加载这个 class
        } catch (ClassNotFoundException e) {
            return;     // 内部类等情况，跳过
        }

        if (!clazz.isAnnotationPresent(Command.class)) return;

        Constructor<?> ctor;
        try {
            ctor = clazz.getDeclaredConstructor();
        } catch (NoSuchMethodException e) {
            // 坑②：子类只有有参构造 → NoSuchMethodException
            throw new KvCommandRegisterException(
                    fqcn + " 必须有无参构造方法（@Command 注解的类不允许构造参数）", e);
        }

        ctor.setAccessible(true);                   // ⭐ 坑①修复：绕过私有构造的访问控制

        Object instance;
        try {
            instance = ctor.newInstance();
        } catch (ReflectiveOperationException e) {
            throw new KvCommandRegisterException(
                    fqcn + " 实例化失败（构造方法内部抛异常？）", e);
        }

        if (!(instance instanceof com.kv.command.Command cmd)) {
            throw new KvCommandRegisterException(
                    fqcn + " 标了 @Command 但没继承 Command 抽象类");
        }

        Command annotation = clazz.getAnnotation(Command.class);
        String name = annotation.name().toUpperCase();
        if (name.isBlank()) {
            throw new KvCommandRegisterException(fqcn + " @Command.name() 不能为空");
        }

        CACHE.put(name, cmd);
        Log.debug("[CommandRegistry] 注册: %s ← %s", name, simpleName);
    }

    public static Optional<com.kv.command.Command> find(String name) {
        return Optional.ofNullable(CACHE.get(name.toUpperCase()));
    }

    public static Set<String> names() { return CACHE.keySet(); }
}
