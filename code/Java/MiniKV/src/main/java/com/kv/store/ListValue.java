package com.kv.store;

import java.util.List;
import java.util.stream.Collectors;

public record ListValue(List<Value> items) implements Value {
    public ListValue {
        items = List.copyOf(items);     // ⭐ 防御性不可变拷贝
    }
    @Override public String typeName() { return "list"; }
    @Override public String toString() {
        return items.stream().map(Value::toString)
                .collect(Collectors.joining(", ", "[", "]"));
    }
}
