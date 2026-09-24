package com.yc.orderforge.domain;

/** 地址：city 可为空。 */
public class Address {
    private final City city;

    public Address(City city) { this.city = city; }

    public City getCity() { return city; }
}
