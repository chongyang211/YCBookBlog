package com.minijson.exception;

/** 必填字段缺失 */
public class JsonKeyMissingException extends JsonException {
    private final String key;
    public JsonKeyMissingException(String key) {
        super("必填 JSON 字段缺失：" + key);
        this.key = key;
    }
    public String getKey() { return key; }
}
