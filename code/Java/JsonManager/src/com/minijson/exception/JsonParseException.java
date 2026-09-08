package com.minijson.exception;

/** 语法解析错误：携带行列号方便定位 */
public class JsonParseException extends JsonException {
    private final int line;
    private final int column;

    public JsonParseException(String msg, int line, int column) {
        super(String.format("解析错误 [行 %d, 列 %d]: %s", line, column, msg));
        this.line = line;
        this.column = column;
    }
    public int getLine()   { return line; }
    public int getColumn() { return column; }
}
