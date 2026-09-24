package flowkit.notify;

/** SMTP 适配器：参数顺序完全不同的另一路"歪"。 */
public class SmtpAdapter implements Channel {
    private final SmtpClient smtp = new SmtpClient();

    @Override
    public void send(String userId, String title, String content) {
        smtp.mail("flowkit@yccoding.com",
                  new String[]{ userId + "@yccoding.com" },
                  "[FlowKit] " + title, content);
        System.out.println("[通知] 邮件 -> " + userId + ": " + title + " · " + content);
    }

    @Override public String name() { return "smtp"; }
}
