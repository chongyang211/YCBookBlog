package com.campus.entity;

import com.campus.enums.Role;

public class Student extends User {
    private String department;
    private String studentNo;

    public Student(String id, String name, String password,
                   String department, String studentNo) {
        super(id, name, password);
        this.department = department;
        this.studentNo = studentNo;
    }

    @Override
    public Role getRole() { return Role.STUDENT; }

    @Override
    public void showMenu() {
        System.out.println("\n===== 学生菜单 =====");
        System.out.println("1. 浏览房间   2. 提交预约");
        System.out.println("3. 我的预约   4. 取消预约");
        System.out.println("0. 退出");
    }

    public String getDepartment() { return department; }
    public String getStudentNo()  { return studentNo; }
}
