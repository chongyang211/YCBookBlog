package com.kv.exception;

public class UnknownCommandException extends KvException {
    public UnknownCommandException(String cmd) {
        super("ERR", "unknown command '" + cmd + "'");
    }
}
