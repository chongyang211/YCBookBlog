package com.campus.entity;

import com.campus.enums.Role;

public class Admin extends User {
    /** 权限位标记：bit 0=审批预约 / bit 1=管理用户 / bit 2=管理房间 */
    private int permBits;

    public Admin(String id, String name, String password, int permBits) {
        super(id, name, password);
        this.permBits = permBits;
    }

    public boolean canApprove()    { return (permBits & 0b001) != 0; }
    public boolean canManageUser() { return (permBits & 0b010) != 0; }
    public boolean canManageRoom() { return (permBits & 0b100) != 0; }

    @Override
    public Role getRole() { return Role.ADMIN; }

    @Override
    public void showMenu() {
        System.out.println("\n===== 管理员菜单 =====");
        if (canApprove())    System.out.println("1. 审批预约");
        if (canManageUser()) System.out.println("2. 增删用户");
        if (canManageRoom()) System.out.println("3. 增删房间");
        System.out.println("4. 最近浏览房间   5. 系统总览");
        System.out.println("0. 退出");
    }
}
