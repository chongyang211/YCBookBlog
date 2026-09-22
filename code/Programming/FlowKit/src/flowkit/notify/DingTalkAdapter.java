package flowkit.notify;

import java.util.Map;

/** 钉钉适配器：把"歪的"（webhook + JSON body）转成"正的"。 */
public class DingTalkAdapter implements Channel {
    private final DingTalkSdk sdk = new DingTalkSdk();
    private final Map<String, String> userWebhooks = Map.of(
        "zhangsan", "https://oapi.dingtalk.com/robot/send?access_token=demo_zhangsan",
        "lisi",     "https://oapi.dingtalk.com/robot/send?access_token=demo_lisi",
        "wangwu",   "https://oapi.dingtalk.com/robot/send?access_token=demo_wangwu",
        "yangc",    "https://oapi.dingtalk.com/robot/send?access_token=demo_yangc",
        "hr1",      "https://oapi.dingtalk.com/robot/send?access_token=demo_hr1",
        "hr2",      "https://oapi.dingtalk.com/robot/send?access_token=demo_hr2");

    @Override
    public void send(String userId, String title, String content) {
        String body = "{\"msgtype\":\"text\",\"text\":{\"content\":"
                    + escape(title + ": " + content) + "}}";
        sdk.pushText(userWebhooks.getOrDefault(userId, "https://oapi.dingtalk.com/robot/send?token=demo"),
                     body);                        // 歪接口在这被掰正
        System.out.println("[通知] 钉钉 -> " + userId + ": " + title + " · " + content);
    }

    @Override public String name() { return "dingtalk"; }

    private static String escape(String s) {
        return s.replace("\\", "\\\\").replace("\"", "\\\"");
    }
}
