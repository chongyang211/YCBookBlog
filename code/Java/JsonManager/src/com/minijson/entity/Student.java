package com.minijson.entity;

import com.minijson.annotation.JsonField;
import com.minijson.annotation.JsonIgnore;

public class Student {
    @JsonField(name = "student_id", required = true)
    public String id;

    @JsonField(required = true)
    public String name;

    public int age;

    @JsonIgnore
    public String password;       // 序列化时跳过

    public Student() {}           // 无参构造（反射需要）

    public Student(String id, String name, int age, String password) {
        this.id = id; this.name = name; this.age = age; this.password = password;
    }

    @Override
    public String toString() {
        return "Student{" + id + ", " + name + ", age=" + age + "}";
    }
}
