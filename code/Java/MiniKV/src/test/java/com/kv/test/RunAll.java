package com.kv.test;

/** 一键全跑：java -cp out com.kv.test.RunAll */
public class RunAll {
    public static void main(String[] args) {
        System.out.println("====== Mini-KV 单元测试 ======");
        TestRunner.runAll(AllTests.class);
    }
}
