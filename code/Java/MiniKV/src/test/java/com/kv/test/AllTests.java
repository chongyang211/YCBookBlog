package com.kv.test;

import com.kv.aof.AofReplayer;
import com.kv.aof.AofWriter;
import com.kv.aof.FlushPolicy;
import com.kv.command.Command;
import com.kv.command.CommandRegistry;
import com.kv.exception.UnknownCommandException;
import com.kv.store.BoolValue;
import com.kv.store.DoubleValue;
import com.kv.store.IntValue;
import com.kv.store.Store;
import com.kv.store.StringValue;
import com.kv.store.Value;

import java.nio.file.Files;
import java.nio.file.Path;
import java.util.Optional;
import java.util.concurrent.CountDownLatch;

/**
 * 10 个单元测试（阶段⑪）—— 覆盖：类型系统 / Store 基础操作 / TTL /
 * 命令注册 / 异常 / AOF round-trip / AOF 半行损坏 / 100 线程并发一致性。
 */
public class AllTests {

    static {
        // AOF 重放走 CommandRegistry.find，先注册命令（幂等，可重复调用）
        CommandRegistry.scanAndRegister("com.kv.command");
    }

    @Test(description = "基础 SET/GET")
    public void testSetGet() {
        Store store = new Store();
        store.set("name", new StringValue("zs"));
        Assert.equals("zs", ((StringValue) store.get("name").get()).s());
    }

    @Test(description = "GET 不存在的 key")
    public void testGetMissing() {
        Store store = new Store();
        Assert.isTrue(store.get("nope").isEmpty(), "应该返回空");
    }

    @Test(description = "DEL 返回正确计数")
    public void testDel() {
        Store store = new Store();
        store.set("k", new IntValue(1));
        Assert.isTrue(store.del("k"), "DEL 应返回 true");
        Assert.isTrue(!store.del("k"), "再删返回 false");
    }

    @Test(description = "TTL 自动过期")
    public void testExpire() throws InterruptedException {
        Store store = new Store();
        store.set("k", new IntValue(1));
        store.expire("k", 0);                    // 立即过期
        Thread.sleep(50);
        Assert.isTrue(store.get("k").isEmpty(), "已过期应读不到");
    }

    @Test(description = "Value parse 类型推断")
    public void testValueParse() {
        Assert.isTrue(Value.parse("true")  instanceof BoolValue,   "true → BoolValue");
        Assert.isTrue(Value.parse("123")   instanceof IntValue,    "123 → IntValue");
        Assert.isTrue(Value.parse("3.14")  instanceof DoubleValue, "3.14 → DoubleValue");
        Assert.isTrue(Value.parse("hello") instanceof StringValue, "hello → StringValue");
    }

    @Test(description = "命令分发：SET 走 SetCmd")
    public void testCommandDispatch() {
        CommandRegistry.scanAndRegister("com.kv.command");
        Optional<Command> cmd = CommandRegistry.find("SET");
        Assert.notNull(cmd.orElse(null), "SET 必须能找到");
        Assert.equals("SET", cmd.get().name());
    }

    @Test(description = "未知命令抛 UnknownCommandException")
    public void testUnknownCommand() {
        Assert.throwsException(UnknownCommandException.class, () -> {
            throw new UnknownCommandException("FOOBAR");
        });
    }

    @Test(description = "AOF round-trip：写后重启数据完整")
    public void testAofRoundTrip() throws Exception {
        Path aof = Files.createTempFile("test-aof-", ".aof");
        try (AofWriter w = new AofWriter(aof, FlushPolicy.EVERY_WRITE)) {
            w.append("SET k1 v1");
            w.append("SET k2 42");
            w.append("EXPIRE k1 3600");
        }

        Store store = new Store();
        AofReplayer.Result r = AofReplayer.replayLenient(store, aof);
        Assert.equals(3, r.totalLines());
        Assert.equals(3, r.successLines());
        Assert.equals("v1", ((StringValue) store.get("k1").get()).s());
        Assert.equals(42L,  ((IntValue) store.get("k2").get()).v());

        Files.delete(aof);
    }

    @Test(description = "100 线程并发一致性")
    public void testConcurrent() throws InterruptedException {
        Store store = new Store();
        int threads = 100, opsPerThread = 1000;
        CountDownLatch done = new CountDownLatch(threads);

        for (int t = 0; t < threads; t++) {
            int id = t;
            new Thread(() -> {
                try {
                    for (int i = 0; i < opsPerThread; i++) {
                        store.set("k" + (id * opsPerThread + i), new IntValue(id));
                    }
                } finally { done.countDown(); }
            }).start();
        }
        done.await();
        Assert.equals(threads * opsPerThread, store.size());
    }

    @Test(description = "损坏 AOF 末行宽松跳过")
    public void testAofCorrupted() throws Exception {
        Path aof = Files.createTempFile("test-aof-corrupted-", ".aof");
        Files.writeString(aof, "SET k1 v1\nGARBAGE\nSET k2 v2\n");

        Store store = new Store();
        AofReplayer.Result r = AofReplayer.replayLenient(store, aof);
        Assert.equals(3, r.totalLines());
        Assert.equals(2, r.successLines());
        Assert.equals(1, r.skippedLines());
        Assert.equals("v1", ((StringValue) store.get("k1").get()).s());
        Assert.equals("v2", ((StringValue) store.get("k2").get()).s());

        Files.delete(aof);
    }
}
