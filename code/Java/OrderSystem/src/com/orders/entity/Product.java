package com.orders.entity;

/** record 天然不可变——多线程共享无需任何保护（教程 §02）*/
public record Product(String id, String name, double price) {
    public Product {
        if (id == null || id.isBlank()) throw new IllegalArgumentException("商品 id 必填");
        if (price < 0) throw new IllegalArgumentException("价格不可为负");
    }
}
