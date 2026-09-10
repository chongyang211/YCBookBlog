package com.orders.service;

/** 库存接口：三种实现横向对比（教程 §08）*/
public interface Inventory {
    int get(String pid);
    boolean deduct(String pid, int qty);
    void set(String pid, int v);
}
