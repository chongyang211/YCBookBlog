package flowkit.domain;

import java.time.Instant;

/**
 * 附件凭证 —— 不可变对象。
 * 呼应正文"思考五"：不可变对象天生免疫浅拷贝坑（引用共享一个不可变对象毫无风险）。
 */
public record Attachment(String filename, Instant time) {
}
