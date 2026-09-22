package flowkit.notify;

/** 模拟第三方钉钉 SDK（假装是外部依赖，改不了）。 */
public class DingTalkSdk {
    public String pushText(String webhookUrl, String body) {
        return "ok";   // 模拟网络往返
    }
}
