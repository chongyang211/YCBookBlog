package flowkit.engine;

import flowkit.domain.Ticket;
import flowkit.notify.EventBus;

import java.time.Duration;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.stream.Collectors;

/**
 * 流程定义 —— 建造者（分步构造 + build() 收口校验）+ 原型（模板克隆）。
 * 定义只读共享，每张工单进来时 instantiate() 出一条各自的审批链。
 */
public class ProcessDef implements ProcessDefApi, Cloneable {
    private final String key;                    // 流程唯一键 如 "refund"
    private final int version;
    private final boolean withdrawable;
    private final List<NodeSpec> specs;          // 图纸（instantiate 时据此铺链/挂装饰）
    private List<NodeDef> nodes;                 // 成品节点（build 时由工厂创建）
    private Map<String, String> edges;           // from -> to

    private ProcessDef(Builder b) {
        this.key = b.key;
        this.version = b.version;
        this.withdrawable = b.withdrawable;
        this.specs = List.copyOf(b.specs);
        this.nodes = List.copyOf(b.nodes);       // 不可变快照，出厂后不可再改
        this.edges = Map.copyOf(b.edges);
    }

    @Override public String key() { return key; }
    public int version() { return version; }
    public boolean withdrawable() { return withdrawable; }
    public List<NodeDef> nodes() { return nodes; }
    public Map<String, String> edges() { return edges; }

    /** 按流程定义铺设当前工单的审批链（工厂出节点 → 装饰器按需包裹 → edges 连线）。 */
    @Override
    public ApprovalContext instantiate(Ticket t, EventBus bus) {
        Map<String, ApproverNode> chain = new HashMap<>();
        for (int i = 0; i < nodes.size(); i++) {
            NodeDef raw = nodes.get(i);                            // 原始 def：取业务数据（finder/expr/members）
            NodeDef decorated = decorate(raw, specs.get(i), bus);  // 装饰后 def：作 enter 挂载点
            ApproverNode n = toChainNode(raw, decorated);
            if (n != null) chain.put(raw.name(), n);
        }
        for (NodeDef def : nodes) {
            ApproverNode n = chain.get(def.name());
            if (n == null) continue;
            String to = edges.get(def.name());                     // 常规连线
            if (to != null && chain.containsKey(to)) n.setNext(chain.get(to));
            if (def.type().equals("CONDITION")) {                  // 条件节点两条边
                ApproverNode trueNode = chain.get(edges.get(def.name() + "-true"));
                ((ConditionNode) n).setTrueNode(trueNode);         // null = true 直接 END
            }
        }
        ApprovalContext ctx = new ApprovalContext();
        ctx.initChain(chain.get(edges.get("START")));
        return ctx;
    }

    /** 按图纸属性给节点"穿衣服"：抄送 → 超时催办（可叠加，先穿的在最里层）。 */
    private NodeDef decorate(NodeDef def, NodeSpec spec, EventBus bus) {
        if (!spec.type().equals("APPROVE")) return def;
        NodeDef d = def;
        if (spec.has("cc")) d = new CcDecorator(d, spec.get("cc", "主管"));
        if (spec.has("timeoutMillis"))
            d = new TimeoutDecorator(d, Duration.ofMillis(spec.get("timeoutMillis", 3000L)), bus);
        return d;
    }

    /** raw 提供业务数据（具体子类），decorated（可能被包裹）作为 enter 的挂载点。 */
    private ApproverNode toChainNode(NodeDef raw, NodeDef decorated) {
        return switch (raw.type()) {
            case "APPROVE"   -> new ApproverNode(raw.name(), ((ApproveNodeDef) raw).finder(), decorated);
            case "AND_SIGN"  -> new GroupNode(raw.name(), true,  ((GroupNodeDef) raw).members(), decorated);
            case "OR_SIGN"   -> new GroupNode(raw.name(), false, ((GroupNodeDef) raw).members(), decorated);
            case "CONDITION" -> new ConditionNode(raw.name(), ((ConditionNodeDef) raw).expr(), decorated);
            default -> null;   // START / END / CC 不上链
        };
    }

    // ---------- 原型：模板克隆 ----------
    // 浅拷贝后 nodes/edges 与原对象共享同一引用 → "改一个，全家跟着变"（§06 的坑）
    @Override public ProcessDef clone() {
        try {
            ProcessDef copy = (ProcessDef) super.clone();      // 浅拷贝！
            copy.nodes = new ArrayList<>(this.nodes);          // 手动深拷贝一层
            copy.edges = new HashMap<>(this.edges);
            return copy;
        } catch (CloneNotSupportedException e) { throw new AssertionError(e); }
    }

    // ---------- 建造者 ----------
    public static class Builder {
        private final String key;
        private int version = 1;
        private boolean withdrawable = true;
        private final List<NodeSpec> specs = new ArrayList<>();
        private final List<NodeDef> nodes = new ArrayList<>();
        private final Map<String, String> edges = new HashMap<>();

        public Builder(String key) { this.key = key; }
        public Builder version(int v)      { this.version = v; return this; }
        public Builder noWithdraw()        { this.withdrawable = false; return this; }
        public Builder node(NodeSpec spec) { this.specs.add(spec); return this; }
        public Builder edge(String from, String to) { this.edges.put(from, to); return this; }

        public ProcessDef build() {
            // 工厂先创建全部成品节点（"节点先全部创建、再统一连线"）
            for (NodeSpec spec : specs) nodes.add(NodeFactories.of(spec.type()).create(spec));
            // 集中校验——Builder 的核心价值：不合法的对象根本"出厂"不了
            long starts = nodes.stream().filter(n -> n.type().equals("START")).count();
            long ends   = nodes.stream().filter(n -> n.type().equals("END")).count();
            if (starts != 1) throw new IllegalStateException(
                "START 节点必须恰好 1 个，实际 " + starts + ": " + key);
            if (ends != 1) throw new IllegalStateException(
                "END 节点必须恰好 1 个，实际 " + ends + ": " + key);
            Set<String> names = specs.stream().map(NodeSpec::name).collect(Collectors.toSet());
            if (names.size() != specs.size())
                throw new IllegalStateException("节点重名: " + key);
            for (var e : edges.entrySet())                     // 连线两端至少一端是已声明节点
                if (!names.contains(e.getKey()) && !names.contains(e.getValue()))
                    throw new IllegalStateException("悬空连线: " + e + ": " + key);
            return new ProcessDef(this);
        }
    }
}
