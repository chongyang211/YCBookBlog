/* 描述表驱动双向映射：本案例最核心的 250 行
 * (char *)obj + fd->offset 定位字段（等价 Field.get/set）
 * 按 fd->type 分派转换（等价拿到 Class<?>）*/
#include "json_mapper.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "json_writer.h"           /* json_escape_write：字符串字段标准转义 */

char *xstrdup(const char *s);      /* json_value.c 提供 */

/* 类型名（描述表打印用）*/
const char *fd_type_name(field_type_t t) {
    switch (t) {
    case FD_STRING:       return "string";
    case FD_INT:          return "int";
    case FD_LONG:         return "long";
    case FD_DOUBLE:       return "double";
    case FD_BOOL:         return "bool";
    case FD_STRUCT:       return "struct";
    case FD_STRUCT_ARRAY: return "struct[]";
    }
    return "unknown";
}

static json_err_t fill_struct(void *obj, const struct_desc_t *desc,
                              const json_value_t *jv, json_error_t *err);

/* 数字节点的宽松取值（int/double 互转）*/
static long num_as_long(const json_value_t *v) {
    return v->as.number_v.is_int ? v->as.number_v.v.i
                                 : (long)v->as.number_v.v.d;
}
static double num_as_double(const json_value_t *v) {
    return v->as.number_v.is_int ? (double)v->as.number_v.v.i
                                 : v->as.number_v.v.d;
}

/* ===== JSON → 结构体 ===== */

static json_err_t fill_field(void *obj, const field_desc_t *fd,
                             const json_value_t *jv, json_error_t *err) {
    char *field = (char *)obj + fd->offset;      /* ⭐ "反射"核心一行 */

    switch (fd->type) {
    case FD_STRING:
        if (jv->type != JSON_STRING)
            RETURN_ERR(err, JSON_ERR_TYPE, 0, 0,
                       "字段 %s 期望 string，实际 %s",
                       fd->field_name, json_type_name(jv->type));
        *(char **)field = xstrdup(jv->as.string_v);
        if (!*(char **)field) return JSON_ERR_MEMORY;
        return JSON_OK;

    case FD_INT:
    case FD_LONG:
    case FD_DOUBLE:
        if (jv->type != JSON_NUMBER)
            RETURN_ERR(err, JSON_ERR_TYPE, 0, 0,
                       "字段 %s 期望 number，实际 %s",
                       fd->field_name, json_type_name(jv->type));
        if (fd->type == FD_INT)         *(int *)field    = (int)num_as_long(jv);
        else if (fd->type == FD_LONG)   *(long *)field   = num_as_long(jv);
        else                            *(double *)field = num_as_double(jv);
        return JSON_OK;

    case FD_BOOL:
        if (jv->type != JSON_BOOL)
            RETURN_ERR(err, JSON_ERR_TYPE, 0, 0,
                       "字段 %s 期望 bool，实际 %s",
                       fd->field_name, json_type_name(jv->type));
        *(bool *)field = jv->as.bool_v;
        return JSON_OK;

    case FD_STRUCT:                                  /* 嵌套：值嵌入，原地递归 */
        return fill_struct(field, fd->sub, jv, err);

    case FD_STRUCT_ARRAY: {
        if (jv->type != JSON_ARRAY)
            RETURN_ERR(err, JSON_ERR_TYPE, 0, 0,
                       "字段 %s 期望 array，实际 %s",
                       fd->field_name, json_type_name(jv->type));
        dynarray_t *arr = (dynarray_t *)field;       /* 布局约定见 json_schema.h */
        json_array_t *ja = jv->as.array_v;
        for (size_t i = 0; i < ja->len; i++) {
            if (arr->len == arr->cap) {
                size_t nc = arr->cap ? arr->cap * 2 : 4;
                void *ni = realloc(arr->items, nc * fd->sub->size);
                if (!ni) return JSON_ERR_MEMORY;
                arr->items = ni;
                arr->cap = nc;
            }
            void *slot = (char *)arr->items + arr->len * fd->sub->size;
            memset(slot, 0, fd->sub->size);          /* 每个元素也"无参构造" */
            json_err_t rc = fill_struct(slot, fd->sub, ja->items[i], err);
            if (rc != JSON_OK) return rc;
            arr->len++;
        }
        return JSON_OK;
    }
    }
    return JSON_ERR_TYPE;
}

static json_err_t fill_struct(void *obj, const struct_desc_t *desc,
                              const json_value_t *jv, json_error_t *err) {
    if (jv->type != JSON_OBJECT)
        RETURN_ERR(err, JSON_ERR_TYPE, 0, 0,
                   "%s 期望 object，实际 %s",
                   desc->struct_name, json_type_name(jv->type));

    for (size_t i = 0; i < desc->field_count; i++) {
        const field_desc_t *fd = &desc->fields[i];
        const json_value_t *val = json_object_get(jv, fd->json_key);
        if (!val) {
            if (fd->required)                        /* ⭐ 必填校验 */
                RETURN_ERR(err, JSON_ERR_KEY_MISSING, 0, 0,
                           "必填 JSON 字段缺失: %s (结构体 %s)",
                           fd->json_key, desc->struct_name);
            continue;                                /* 可选缺失 → 保持零值 */
        }
        json_err_t rc = fill_field(obj, fd, val, err);
        if (rc != JSON_OK) return rc;
    }
    return JSON_OK;
}

json_err_t json_to_struct(void *obj, const struct_desc_t *desc,
                          const json_value_t *jv, json_error_t *err) {
    return fill_struct(obj, desc, jv, err);
}

/* ===== 结构体 → JSON ===== */

static void emit_field(strbuf_t *out, const void *obj,
                       const field_desc_t *fd);

static void emit_struct(strbuf_t *out, const void *obj,
                        const struct_desc_t *desc) {
    strbuf_append_ch(out, '{');
    for (size_t i = 0; i < desc->field_count; i++) {
        if (i) strbuf_append_ch(out, ',');
        strbuf_append_ch(out, '"');
        strbuf_append(out, desc->fields[i].json_key);
        strbuf_append(out, "\":");
        emit_field(out, obj, &desc->fields[i]);
    }
    strbuf_append_ch(out, '}');
}

static void emit_field(strbuf_t *out, const void *obj,
                       const field_desc_t *fd) {
    const char *field = (const char *)obj + fd->offset;   /* 只读版"反射" */

    switch (fd->type) {
    case FD_STRING: {
        const char *s = *(char *const *)field;
        strbuf_append_ch(out, '"');
        json_escape_write(out, s ? s : "");   /* 复用 writer 的标准转义（挑战 B②）*/
        strbuf_append_ch(out, '"');
        break;
    }
    case FD_INT:    strbuf_append_fmt(out, "%d",  *(const int *)field);    break;
    case FD_LONG:   strbuf_append_fmt(out, "%ld", *(const long *)field);   break;
    case FD_DOUBLE: strbuf_append_fmt(out, "%g",  *(const double *)field); break;
    case FD_BOOL:   strbuf_append(out, *(const bool *)field ? "true" : "false"); break;
    case FD_STRUCT:
        emit_struct(out, field, fd->sub);                 /* 递归 */
        break;
    case FD_STRUCT_ARRAY: {
        const dynarray_t *arr = (const dynarray_t *)field;
        strbuf_append_ch(out, '[');
        for (size_t i = 0; i < arr->len; i++) {
            if (i) strbuf_append_ch(out, ',');
            emit_struct(out, (const char *)arr->items + i * fd->sub->size,
                        fd->sub);                         /* 递归每个元素 */
        }
        strbuf_append_ch(out, ']');
        break;
    }
    }
}

json_err_t struct_to_json(strbuf_t *out, const void *obj,
                          const struct_desc_t *desc) {
    emit_struct(out, obj, desc);
    return JSON_OK;
}

/* ===== new / free / clone 三件套 ===== */

void *struct_new(const struct_desc_t *desc) {
    return calloc(1, desc->size);          /* ⭐ calloc = 无参构造（零初始化）*/
}

/* 只释放 obj 的字段持有的堆内存，不 free(obj) 本身——递归专用：
 * FD_STRUCT 是嵌入父结构体的偏移地址、FD_STRUCT_ARRAY 的元素嵌在
 * items 数组内部，它们都不是独立 malloc 的块，绝不能对它们 free
 * （items 数组本体在这里释放；元素/嵌入体只释放自己的字段）*/
static void struct_free_fields(void *obj, const struct_desc_t *desc) {
    for (size_t i = 0; i < desc->field_count; i++) {
        const field_desc_t *fd = &desc->fields[i];
        char *field = (char *)obj + fd->offset;
        switch (fd->type) {
        case FD_STRING:
            free(*(char **)field);         /* 释放字符串 */
            break;
        case FD_STRUCT:
            struct_free_fields(field, fd->sub);   /* 嵌入：只释放其字段 */
            break;
        case FD_STRUCT_ARRAY: {
            dynarray_t *arr = (dynarray_t *)field;
            for (size_t j = 0; j < arr->len; j++)
                struct_free_fields((char *)arr->items + j * fd->sub->size,
                                   fd->sub);      /* 元素：只释放其字段 */
            free(arr->items);                     /* 数组本体在这里释放 */
            break;
        }
        default: break;                    /* 标量无堆内存 */
        }
    }
}

void struct_free(void *obj, const struct_desc_t *desc) {
    if (!obj) return;
    struct_free_fields(obj, desc);
    free(obj);
}

static void clone_field(void *dst, const void *src, const field_desc_t *fd);

static void clone_struct(void *dst, const void *src,
                         const struct_desc_t *desc) {
    for (size_t i = 0; i < desc->field_count; i++)
        clone_field(dst, src, &desc->fields[i]);
}

static void clone_field(void *dst, const void *src, const field_desc_t *fd) {
    char *d = (char *)dst + fd->offset;
    const char *s = (const char *)src + fd->offset;

    switch (fd->type) {
    case FD_STRING:
        *(char **)d = xstrdup(*(char *const *)s ? *(char *const *)s : "");
        break;
    case FD_INT:    *(int *)d    = *(const int *)s;    break;
    case FD_LONG:   *(long *)d   = *(const long *)s;   break;
    case FD_DOUBLE: *(double *)d = *(const double *)s; break;
    case FD_BOOL:   *(bool *)d   = *(const bool *)s;   break;
    case FD_STRUCT:
        clone_struct(d, s, fd->sub);                   /* 递归深拷贝 */
        break;
    case FD_STRUCT_ARRAY: {
        const dynarray_t *sa = (const dynarray_t *)s;
        dynarray_t *da = (dynarray_t *)d;
        *da = (dynarray_t){0};
        da->items = calloc(sa->len ? sa->len : 1, fd->sub->size);
        da->cap = da->len = sa->len;
        for (size_t j = 0; j < sa->len; j++)
            clone_struct((char *)da->items + j * fd->sub->size,
                         (const char *)sa->items + j * fd->sub->size,
                         fd->sub);
        break;
    }
    }
}

void *struct_clone(const void *obj, const struct_desc_t *desc) {
    void *copy = struct_new(desc);
    if (copy) clone_struct(copy, obj, desc);
    return copy;
}
