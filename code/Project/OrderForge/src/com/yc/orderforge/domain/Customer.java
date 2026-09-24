package com.yc.orderforge.domain;

/** 顾客：address 可为空（游客/新用户未填地址）。 */
public class Customer {
    private final String name;
    private final String phone;
    private final Address address;

    public Customer(String name, String phone, Address address) {
        this.name = name;
        this.phone = phone;
        this.address = address;
    }

    public String getName()    { return name; }
    public String getPhone()   { return phone; }
    public Address getAddress(){ return address; }
}
