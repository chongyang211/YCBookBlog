/* 展开 2：生成字段描述表 —— 字段清单见 entity.h 的 *_FIELDS_X
 * offsetof 由 FIELD 宏编译期计算（手写 offset 会内存踩踏，教程 §8.3）
 * 加字段只改 entity.h 的清单，本文件零改动 */
#include "entity.h"

#define X(t, cname, jkey, ty, req) \
    FIELD(student_t, cname, jkey, ty, req),
static const field_desc_t STUDENT_FIELDS[] = { STUDENT_FIELDS_X };
#undef X

const struct_desc_t STUDENT_DESC = {
    .struct_name = "student_t",
    .size        = sizeof(student_t),
    .field_count = sizeof STUDENT_FIELDS / sizeof STUDENT_FIELDS[0],
    .fields      = STUDENT_FIELDS,
};

#define X2(t, cname, jkey, ty, req, sub) \
    FIELD_SUB(course_t, cname, jkey, ty, req, sub),
static const field_desc_t COURSE_FIELDS[] = { COURSE_FIELDS_X };
#undef X2

const struct_desc_t COURSE_DESC = {
    .struct_name = "course_t",
    .size        = sizeof(course_t),
    .field_count = sizeof COURSE_FIELDS / sizeof COURSE_FIELDS[0],
    .fields      = COURSE_FIELDS,
};
