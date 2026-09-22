package flowkit.notify;

/** 通知失败异常（适配器把各渠道的失败语义统一成它）。 */
public class NotifyException extends RuntimeException {
    public NotifyException(String message) { super(message); }
}
