package flowkit.engine;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * 节点"图纸" —— 可变的中间载体（链式 setter），NodeDef 才是"成品"。
 * "规格对象的链式 setter + Builder 分步装配 + build() 收口校验"
 * 是建造者模式在真实工程里的标准三件套。
 */
public class NodeSpec {
    private final String type;                       // APPROVE / AND_SIGN / CONDITION ...
    private final String name;                       // 节点名（流程内唯一，连线靠它）
    private final Map<String, Object> attrs = new HashMap<>();

    private NodeSpec(String type, String name) { this.type = type; this.name = name; }

    public static NodeSpec start()    { return new NodeSpec("START", "START"); }
    public static NodeSpec end()      { return new NodeSpec("END", "END"); }
    public static NodeSpec approve(String name)   { return new NodeSpec("APPROVE", name); }
    public static NodeSpec condition(String name) { return new NodeSpec("CONDITION", name); }
    public static NodeSpec cc(String name)        { return new NodeSpec("CC", name); }
    public static NodeSpec andSign(String name, String... members) {
        return new NodeSpec("AND_SIGN", name).attr("members", List.of(members));
    }
    public static NodeSpec orSign(String name, String... members) {
        return new NodeSpec("OR_SIGN", name).attr("members", List.of(members));
    }

    public NodeSpec finder(String finderName) { attrs.put("finder", finderName); return this; }
    public NodeSpec person(String person)     { attrs.put("person", person); return this; }
    public NodeSpec expr(String source)       { attrs.put("expr", source); return this; }
    public NodeSpec ccTo(String ccTo)         { attrs.put("cc", ccTo); return this; }
    public NodeSpec timeoutMillis(long ms)    { attrs.put("timeoutMillis", ms); return this; }
    public NodeSpec attr(String k, Object v)  { attrs.put(k, v); return this; }

    public String type()   { return type; }
    public String name()   { return name; }
    public String finder() { return (String) attrs.get("finder"); }
    public String person() { return (String) attrs.get("person"); }
    public String expr()   { return (String) attrs.get("expr"); }

    @SuppressWarnings("unchecked")
    public <T> T get(String k, T def) {
        return attrs.containsKey(k) ? (T) attrs.get(k) : def;
    }

    public boolean has(String k) { return attrs.containsKey(k); }

    @SuppressWarnings("unchecked")
    public List<String> members() {
        Object m = attrs.get("members");
        return m == null ? List.of() : (List<String>) m;
    }
}
