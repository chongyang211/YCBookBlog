package com.yc.orderforge.metrics;

import java.nio.file.Files;
import java.nio.file.Path;
import java.util.List;

/**
 * V0 体检报告（会话 2）：对上帝类做圈复杂度 + 坏味道扫描。
 * 对应 08 篇「圈复杂度体温计 + 22 种坏味道清单」。
 *
 * 运行：./run.sh metrics
 */
public class MetricsReport {

    private static final String V0 = "src/com/yc/orderforge/v0god/OrderManager.java";

    public static void main(String[] args) throws Exception {
        String src = Files.readString(Path.of(V0));

        System.out.println("================ V0 上帝类体检报告 ================");
        System.out.println("对象: " + V0);
        System.out.println("总行数(非空): " + CodeMetrics.loc(src));
        System.out.println();

        System.out.println("---- 方法级圈复杂度（判断节点数 + 1）----");
        System.out.println(String.format("%-18s %8s %10s  %s", "方法", "行数", "圈复杂度", "风险"));
        List<CodeMetrics.MethodInfo> methods = CodeMetrics.methods(src);
        int maxCc = 0;
        for (CodeMetrics.MethodInfo m : methods) {
            maxCc = Math.max(maxCc, m.cyclomatic);
            String risk = m.cyclomatic > 20 ? "极高(>50 不可维护 / >20 必须重构)"
                     : m.cyclomatic >= 10 ? "高(>=10 必须重构)"
                     : m.cyclomatic >= 5  ? "中(考虑简化)" : "低";
            System.out.println(String.format("%-18s %8d %10d  %s", m.name, m.loc, m.cyclomatic, risk));
        }
        System.out.println();

        System.out.println("---- 坏味道扫描（对照 08 篇 22 种清单）----");
        List<String> smells = CodeMetrics.smellScan("OrderManager", src);
        for (int i = 0; i < smells.size(); i++) {
            System.out.println("  " + (i + 1) + ". " + smells.get(i));
        }
        System.out.println();

        System.out.println("---- 结论（08 篇体温计）----");
        System.out.println("  最大方法圈复杂度 = " + maxCc
            + (maxCc >= 10 ? "  → 越过 10 警戒线，必须重构" : "  → 尚在安全区"));
        System.out.println("  坏味道命中 " + smells.size() + " 条");
        System.out.println("  诊断：过大的类 + 上帝方法 + 全局状态 + 火车残骸 + 隐式 IO");
        System.out.println("  手术方案：V1 拆类(SRP) → V2 策略链(OCP) → V3 契约+角色接口(LSP/ISP) → V4 注入+门面(DIP/LOD)");
    }
}
