package com.kv.exception;

public class AofCorruptedException extends KvException {
    public AofCorruptedException(String message) {
        super("AOFERR", message);
    }
    public AofCorruptedException(String message, Throwable cause) {
        super("AOFERR", message, cause);
    }
}
