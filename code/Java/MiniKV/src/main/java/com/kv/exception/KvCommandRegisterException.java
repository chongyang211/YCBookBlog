package com.kv.exception;

public class KvCommandRegisterException extends KvException {
    public KvCommandRegisterException(String message) { super("REGERR", message); }
    public KvCommandRegisterException(String message, Throwable cause) {
        super("REGERR", message, cause);
    }
}
