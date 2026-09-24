package flowkit.engine;

import java.util.List;

/** 会签/或签节点定义：成员名单随定义走。 */
public record GroupNodeDef(String name, boolean andMode, List<String> members) implements NodeDef {
    @Override public String type() { return andMode ? "AND_SIGN" : "OR_SIGN"; }
}
