package flowkit.stats;

import flowkit.task.AndTaskGroup;
import flowkit.task.OrTaskGroup;
import flowkit.task.SingleTask;

import java.util.ArrayList;
import java.util.List;

/** 运营视角：节点耗时 P95。 */
public class SlmVisitor implements TaskVisitor {
    private final List<Long> nodeMillis = new ArrayList<>();

    @Override public void visit(SingleTask t) { nodeMillis.add(t.elapsedMillis()); }
    @Override public void visit(AndTaskGroup g) { }
    @Override public void visit(OrTaskGroup g)  { }

    public long p95() {
        return nodeMillis.stream().sorted()
                         .skip(nodeMillis.size() * 95 / 100)
                         .findFirst().orElse(0L);
    }
}
