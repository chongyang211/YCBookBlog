/* 字段描述表：宏 + offsetof = 编译期"手写反射"（本案例灵魂）
 *
 * Java 的 @JsonField 注解 → FIELD 宏；Java 的 Class<T> → struct_desc_t；
 * Java 的 Field.get/set → (char *)obj + fd->offset
 * 不写进描述表的字段 = 双向忽略（对应 @JsonIgnore）*/
#ifndef JSON_SCHEMA_H
#define JSON_SCHEMA_H

#include <stdbool.h>
#include <stddef.h>

/* ===== 字段类型（对应 Java 版反射拿到的 Class<?>）===== */
typedef enum {
    FD_STRING,        /* char*  （malloc，所有权归结构体）*/
    FD_INT,
    FD_LONG,
    FD_DOUBLE,
    FD_BOOL,
    FD_STRUCT,        /* 嵌套结构体（值嵌入），sub 指向子描述 */
    FD_STRUCT_ARRAY   /* 嵌套结构体数组，字段布局约定见下 */
} field_type_t;

typedef struct struct_desc struct_desc_t;

/* ===== 字段描述（对应 Java 的 Field + @JsonField）===== */
typedef struct {
    const char *field_name;    /* C 字段名（#字符串化生成）*/
    const char *json_key;      /* JSON key（对应 @JsonField(name)）*/
    field_type_t type;
    size_t      offset;        /* ⭐ offsetof 编译期算出 */
    bool        required;      /* 对应 @JsonField(required) */
    const struct_desc_t *sub;  /* FD_STRUCT/FD_STRUCT_ARRAY 用 */
} field_desc_t;

/* ===== 结构体描述（对应 Java 的 Class<T>）===== */
struct struct_desc {
    const char         *struct_name;
    size_t              size;          /* sizeof —— calloc 当"无参构造"用 */
    size_t              field_count;
    const field_desc_t *fields;
};

/* ===== 声明宏：offsetof 让 offset 永远不可能写错 ===== */
#define FIELD(struct_t, cname, jkey, ty, req)                          \
    { #cname, (jkey), (ty), offsetof(struct_t, cname), (req), NULL }

#define FIELD_SUB(struct_t, cname, jkey, ty, req, subdesc)             \
    { #cname, (jkey), (ty), offsetof(struct_t, cname), (req), (subdesc) }

/* FD_STRUCT_ARRAY 字段的内存布局约定（必须与实体里的数组结构一致）：
 *   typedef struct { <实体类型> *items; size_t len, cap; } xxx_array_t;
 * items/len/cap 三个成员的名字与顺序不可变（mapper 按 dynarray_t 解释）*/
typedef struct { void *items; size_t len, cap; } dynarray_t;

const char *fd_type_name(field_type_t t);

#endif
