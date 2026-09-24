package flowkit.ui;

import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

/**
 * 享元池 —— 本质是带缓存的工厂。
 * 不变的东西共享（同类型图标全屏只有一个实例），可变的东西当参数传。
 */
public class IconPool {
    private static final Map<String, Icon> POOL = new ConcurrentHashMap<>();

    private IconPool() {}

    public static Icon of(String kind) {
        boolean hit = POOL.containsKey(kind);                    // 教学观测点
        Icon icon = POOL.computeIfAbsent(kind, k -> new Icon(load(k)));
        System.out.println((hit ? "[HIT]  " : "[MISS] ") + kind + "  池内种类=" + POOL.size());
        return icon;
    }

    /** 模拟从资源包加载像素数据。 */
    private static byte[] load(String kind) {
        return new byte[4096];       // 每张 4KB，真实加载成本越高享元越值
    }
}
