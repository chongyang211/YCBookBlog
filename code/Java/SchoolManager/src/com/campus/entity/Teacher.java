package com.campus.entity;

import com.campus.enums.Role;

public class Teacher extends User {
    private String department;
    private String title;        // 讲师 / 副教授 / 教授

    public Teacher(String id, String name, String password,
                   String department, String title) {
        super(id, name, password);
        this.department = department;
        this.title = title;
    }

    @Override
    public Role getRole() { return Role.TEACHER; }

    @Override
    public void showMenu() {
        System.out.println("\n===== 教师菜单 =====");
        System.out.println("1. 发布演讲   2. 演讲 Top-K");
        System.out.println("3. 按系别平均分");
        System.out.println("0. 退出");
    }

    public String getDepartment() { return department; }
    public String getTitle()      { return title; }
}
