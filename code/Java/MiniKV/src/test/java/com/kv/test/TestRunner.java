package com.kv.test;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Comparator;
import java.util.List;

/**
 * 自写测试运行器（阶段⑪）—— JUnit 5 的核心原理：
 * 反射扫所有方法 → 按 @Test 注解过滤 → 每个方法 new 一个实例（测试隔离）→ 反射 invoke。
 *
 * 失败时 System.exit(1) → CI/CD 能识别退出码。
 */
public class TestRunner {

    public static void runAll(Class<?> testClass) {
        int total = 0, passed = 0, failed = 0;
        List<String> failures = new ArrayList<>();

        Method[] methods = testClass.getDeclaredMethods();
        Arrays.sort(methods, Comparator.comparing(Method::getName));

        for (Method method : methods) {
            if (!method.isAnnotationPresent(Test.class)) continue;
            total++;
            method.setAccessible(true);

            String desc = method.getAnnotation(Test.class).description();
            String label = method.getName() + (desc.isEmpty() ? "" : " (" + desc + ")");

            try {
                // ⭐ 每个测试 new 一个实例 = 天然隔离，无需 tearDown
                Object instance = testClass.getDeclaredConstructor().newInstance();
                method.invoke(instance);
                System.out.println("  ✅ " + label);
                passed++;
            } catch (InvocationTargetException e) {
                Throwable cause = e.getCause();
                System.out.println("  ❌ " + label);
                System.out.println("     " + cause.getClass().getSimpleName() + ": " + cause.getMessage());
                failures.add(label + " → " + cause.getMessage());
                failed++;
            } catch (Exception e) {
                System.out.println("  ⚠️ " + label + " 框架错误: " + e);
                failed++;
            }
        }

        System.out.println();
        System.out.printf("==> 共 %d / 通过 %d / 失败 %d%n", total, passed, failed);
        if (failed > 0) {
            System.out.println("失败列表:");
            failures.forEach(s -> System.out.println("  - " + s));
            System.exit(1);          // 失败时返回非零 → CI/CD 能识别
        }
    }
}
