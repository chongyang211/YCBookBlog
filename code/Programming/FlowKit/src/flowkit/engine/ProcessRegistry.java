package flowkit.engine;

/**
 * 流程注册中心 —— 全项目唯一的单例（静态内部类版：懒加载 + 线程安全）。
 * OrgService / RoleService 等都是普通对象由调用方持有：
 * "全局只有一份" != "必须用单例"，单例隐藏依赖、妨碍测试。
 */
public class ProcessRegistry {
    private final java.util.Map<String, ProcessDefApi> defs = new java.util.concurrent.ConcurrentHashMap<>();

    private ProcessRegistry() { loadDefaults(); }

    private static class Holder { static final ProcessRegistry INSTANCE = new ProcessRegistry(); }
    public static ProcessRegistry getInstance() { return Holder.INSTANCE; }

    public void register(String key, ProcessDefApi def) { defs.put(key, def); }

    public ProcessDefApi get(String key) {
        ProcessDefApi d = defs.get(key);
        if (d == null) throw new IllegalArgumentException("未注册流程: " + key);
        return d;
    }

    public java.util.Set<String> keys() { return java.util.Set.copyOf(defs.keySet()); }

    private void loadDefaults() {
        register("refund", refundProcess());
        register("leave", leaveProcess());
        register("expense", expenseProcess());

        // 原型：基于 expense 模板克隆微调（深拷贝后互不影响）
        ProcessDef v2 = expenseProcess().clone();
        v2.nodes().add(new SimpleNodeDef("审计抄送", "CC"));   // 只有克隆版多一个节点
        register("expense_v2", v2);

        // 代理：罕见流程懒加载（首单才付加载成本）
        register("rare_process", new LazyProcessProxy("rare_process", ProcessRegistry::rareProcess));
    }

    /** 默认退款流程：组长(leader) → 主管(固定人) → 金额路由 → 财务总监(finance) → END。 */
    private static ProcessDef refundProcess() {
        return new ProcessDef.Builder("refund")
            .node(NodeSpec.start())
            .node(NodeSpec.approve("直销组长").finder("leader").ccTo("主管"))
            .node(NodeSpec.approve("主管").person("lisi").timeoutMillis(3000))
            .node(NodeSpec.condition("金额路由").expr("amount > 5000"))
            .node(NodeSpec.approve("财务总监").finder("finance"))
            .node(NodeSpec.end())
            .edge("START", "直销组长").edge("直销组长", "主管")
            .edge("主管", "金额路由")
            .edge("金额路由-true", "财务总监").edge("金额路由-false", "END")
            .edge("财务总监", "END")
            .build();
    }

    /** 请假流程：组长 → HR 会签（两人全过才放行）。 */
    private static ProcessDef leaveProcess() {
        return new ProcessDef.Builder("leave")
            .node(NodeSpec.start())
            .node(NodeSpec.approve("直销组长").finder("leader"))
            .node(NodeSpec.andSign("HR会签", "hr1", "hr2"))
            .node(NodeSpec.end())
            .edge("START", "直销组长").edge("直销组长", "HR会签").edge("HR会签", "END")
            .build();
    }

    /** 报销流程（作为原型克隆的模板）：主管（金额阶梯策略）。 */
    private static ProcessDef expenseProcess() {
        return new ProcessDef.Builder("expense")
            .node(NodeSpec.start())
            .node(NodeSpec.approve("主管").finder("ladder"))
            .node(NodeSpec.end())
            .edge("START", "主管").edge("主管", "END")
            .build();
    }

    /** 罕见流程：走懒加载代理，启动时零成本。 */
    private static ProcessDef rareProcess() {
        return new ProcessDef.Builder("rare_process")
            .node(NodeSpec.start())
            .node(NodeSpec.approve("总监").finder("leader"))
            .node(NodeSpec.end())
            .edge("START", "总监").edge("总监", "END")
            .build();
    }
}
