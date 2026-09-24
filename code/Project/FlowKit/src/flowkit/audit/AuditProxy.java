package flowkit.audit;

import java.lang.reflect.InvocationHandler;
import java.lang.reflect.Method;
import java.lang.reflect.Proxy;
import java.util.Arrays;

/**
 * JDK 动态代理 —— "所有审批动作都要留痕，且不想在 40 个方法里手写日志"。
 * 一行包裹，全接口所有方法自动留痕（Spring AOP / @Transactional 同原理）。
 */
public class AuditProxy implements InvocationHandler {
    private final Object target;

    private AuditProxy(Object target) { this.target = target; }

    @SuppressWarnings("unchecked")
    public static <T> T wrap(T target) {
        return (T) Proxy.newProxyInstance(
            target.getClass().getClassLoader(),
            target.getClass().getInterfaces(),
            new AuditProxy(target));
    }

    @Override
    public Object invoke(Object proxy, Method m, Object[] args) throws Throwable {
        long start = System.nanoTime();
        try {
            return m.invoke(target, args);                       // 真正的调用
        } finally {
            System.out.printf("[审计] %s#%s 耗时=%dµs args=%s%n",
                target.getClass().getSimpleName(), m.getName(),
                (System.nanoTime() - start) / 1000, Arrays.toString(args));
        }
    }
}
