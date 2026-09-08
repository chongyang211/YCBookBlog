package com.minijson.exception;

/** JSON 体系所有异常的根 */
public class JsonException extends RuntimeException {
    public JsonException(String msg)               { super(msg); }
    public JsonException(String msg, Throwable t)  { super(msg, t); }
}
