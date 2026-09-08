package com.campus.entity;

import com.campus.enums.Role;

/**
 * 用户抽象基类：公共字段 + 抽象方法（强制子类实现）。
 */
public abstract class User {
    protected final String id;
    protected String name;
    protected String password;

    protected User(String id, String name, String password) {
        this.id = id;
        this.name = name;
        this.password = password;
    }

    // ===== 公共 getter =====
    public String getId() { return id; }
    public String getName() { return name; }
    public void setName(String name) { this.name = name; }
    public String getPassword() { return password; }
    public boolean checkPassword(String input) {
        return password != null && password.equals(input);
    }

    // ===== 抽象方法（强制子类实现）=====
    public abstract Role getRole();
    public abstract void showMenu();

    // ===== Object 三件套 =====
    @Override
    public String toString() {
        return String.format("%s[id=%s, name=%s, role=%s]",
                getClass().getSimpleName(), id, name, getRole());
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (!(o instanceof User u)) return false;
        return id.equals(u.id);
    }

    @Override
    public int hashCode() { return id.hashCode(); }
}
