package com.minijson.entity;

import com.minijson.annotation.JsonField;

import java.util.List;

public class Course {
    @JsonField(required = true)
    public String code;

    public String name;

    public List<Student> students;       // 泛型集合（测试类型擦除突破）

    public Course() {}

    public Course(String code, String name, List<Student> students) {
        this.code = code; this.name = name; this.students = students;
    }

    @Override
    public String toString() {
        return "Course{" + code + ", " + name + ", students=" + students + "}";
    }
}
