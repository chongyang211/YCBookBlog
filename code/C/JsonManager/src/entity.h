/* X-Macro 实体声明（教程挑战 C）：一份字段清单生成"结构体 + 描述表"两样
 *
 * 痛点：结构体定义与 FIELDS[] 描述表两处维护——加字段要改两处，漏一处就不同步
 * 方案：字段清单（STUDENT_FIELDS_X）只写一遍，两个展开点各自定义 X 再展开：
 *   entity.h  展开 1 → 结构体成员；entity.c 展开 2 → field_desc_t 数组
 * 这就是 Linux 内核与 Protobuf C 代码生成器的日常。*/
#ifndef ENTITY_H
#define ENTITY_H

#include "json_schema.h"

/* ===== 唯一的"事实源"：字段清单 ===== */

/* 学生表：5 参 X（类型, C字段名, json key, 字段类型, 必填）*/
#define STUDENT_FIELDS_X \
    X(char*, id,   "student_id", FD_STRING, true)     \
    X(char*, name, "name",       FD_STRING, true)     \
    X(int,   age,  "age",        FD_INT,    false)
/* char *password —— 不写进清单 = 双向忽略（对应 @JsonIgnore）*/

/* 课程表：带嵌套字段，用 6 参 X2（多一个 sub 描述指针，NULL = 无嵌套）*/
#define COURSE_FIELDS_X \
    X2(char*,           code,     "code",     FD_STRING,       true,  NULL)          \
    X2(char*,           name,     "name",     FD_STRING,       false, NULL)          \
    X2(student_array_t, students, "students", FD_STRUCT_ARRAY, false, &STUDENT_DESC)

/* ===== 展开 1：生成结构体定义 ===== */

#define X(t, cname, jkey, ty, req) t cname;
typedef struct { STUDENT_FIELDS_X } student_t;
#undef X

/* 学生数组（布局约定：items/len/cap，mapper 按 dynarray_t 解释）*/
typedef struct {
    student_t *items;
    size_t len, cap;
} student_array_t;

#define X2(t, cname, jkey, ty, req, sub) t cname;
typedef struct { COURSE_FIELDS_X } course_t;      /* students 对应 Java List<Student> */
#undef X2

extern const struct_desc_t STUDENT_DESC;
extern const struct_desc_t COURSE_DESC;

#endif
