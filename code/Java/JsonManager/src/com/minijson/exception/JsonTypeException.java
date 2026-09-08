package com.minijson.exception;

/** 类型不匹配（期望 X 实际 Y） */
public class JsonTypeException extends JsonException {
    private final String expected;
    private final String actual;

    public JsonTypeException(String expected, String actual) {
        super(String.format("类型不匹配：期望 %s，实际 %s", expected, actual));
        this.expected = expected;
        this.actual = actual;
    }
    public String getExpected() { return expected; }
    public String getActual()   { return actual; }
}
