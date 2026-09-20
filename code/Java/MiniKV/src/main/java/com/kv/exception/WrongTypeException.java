package com.kv.exception;

public class WrongTypeException extends KvException {
    public WrongTypeException(String key, String expected, String actual) {
        super("WRONGTYPE", "Operation against a key holding the wrong kind of value (key=" +
                key + ", expected=" + expected + ", actual=" + actual + ")");
    }
}
