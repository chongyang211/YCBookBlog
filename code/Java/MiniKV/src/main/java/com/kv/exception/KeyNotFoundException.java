package com.kv.exception;

public class KeyNotFoundException extends KvException {
    public KeyNotFoundException(String key) {
        super("ERR", "no such key: " + key);
    }
}
