package flowkit.notify;

/** 企微适配器：吞掉返回码差异，统一成异常语义。 */
public class WeComAdapter implements Channel {
    private final WeComSdk sdk = new WeComSdk();
    private final String corpId = "ww_demo_corp";
    // 密钥只从环境变量来（生产铁律）；教学版允许 demo 兜底以便离线演示
    private final String corpSecret = envSecret();

    private static String envSecret() {
        String s = System.getenv("WECOM_SECRET");
        return (s == null || s.isBlank()) ? "demo-secret-教学模拟" : s;
    }

    @Override
    public void send(String userId, String title, String content) {
        int code = sdk.send(corpId, corpSecret, title + "\n" + content);
        if (code != 0)
            throw new NotifyException("企微发送失败 code=" + code + " user=" + userId);
        System.out.println("[通知] 企微 -> " + userId + ": " + title + " · " + content);
    }

    @Override public String name() { return "wecom"; }
}
