package flowkit.notify;

/**
 * 适配器目标接口 —— 我们想要的形状。
 * 调用方代码里从此搜不到 webhook / corpSecret / from 这些三方词汇。
 */
public interface Channel {
    void send(String userId, String title, String content);
    String name();
}
