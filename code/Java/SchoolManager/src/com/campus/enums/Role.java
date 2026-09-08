package com.campus.enums;

public enum Role {
    STUDENT("学生"),
    TEACHER("教师"),
    ADMIN("管理员");

    private final String displayName;

    Role(String displayName) {
        this.displayName = displayName;
    }

    public String getDisplayName() { return displayName; }

    /** 从字符串解析（CSV 反序列化用） */
    public static Role fromString(String s) {
        for (Role r : values()) {
            if (r.name().equalsIgnoreCase(s)) return r;
        }
        throw new IllegalArgumentException("未知 Role: " + s);
    }
}
