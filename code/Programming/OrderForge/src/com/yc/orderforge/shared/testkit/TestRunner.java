package com.yc.orderforge.shared.testkit;

import java.util.ArrayList;
import java.util.List;

/** 极简测试运行器：链式注册用例 → run() 打印结果，失败退出码 1。 */
public final class TestRunner {
    private final String suiteName;
    private final List<Case> cases = new ArrayList<>();

    public TestRunner(String suiteName) { this.suiteName = suiteName; }

    public TestRunner test(String name, Executable body) {
        cases.add(new Case(name, body));
        return this;
    }

    public void run() {
        System.out.println("[TEST SUITE] " + suiteName);
        int pass = 0, fail = 0;
        long start = System.currentTimeMillis();
        for (Case c : cases) {
            try {
                c.body.execute();
                System.out.println("  ✓ " + c.name);
                pass++;
            } catch (Throwable t) {
                System.out.println("  ✗ " + c.name);
                System.out.println("      " + t.getMessage());
                fail++;
            }
        }
        System.out.println(String.format("[结果] %d 通过 / %d 失败 / %dms",
            pass, fail, System.currentTimeMillis() - start));
        if (fail > 0) System.exit(1);
    }

    private static final class Case {
        final String name; final Executable body;
        Case(String name, Executable body) { this.name = name; this.body = body; }
    }
}
