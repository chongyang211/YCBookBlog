#ifndef JSON_VALUE_H
#define JSON_VALUE_H

#include <stdbool.h>
#include <stddef.h>

/* ===== 类型标签（sealed：只允许这 6 种）===== */
typedef enum {
    JSON_NULL = 0,
    JSON_BOOL,
    JSON_NUMBER,
    JSON_STRING,
    JSON_ARRAY,
    JSON_OBJECT
} json_type_t;

typedef struct json_value json_value_t;

/* 数字：整数/浮点二选一，long 保精确、double 保范围 */
typedef struct {
    bool is_int;
    union { long i; double d; } v;
} json_number_t;

/* 数组：动态数组三件套 */
typedef struct {
    json_value_t **items;
    size_t len, cap;
} json_array_t;

/* 对象：keys 与 values 平行数组——天然保序（对应 Java LinkedHashMap）*/
typedef struct {
    char **keys;
    json_value_t **values;
    size_t len, cap;
} json_object_t;

/* ===== AST 节点本体（tagged union）===== */
struct json_value {
    json_type_t type;              /* ⭐ 标签：union 当前是什么 */
    union {
        bool            bool_v;
        json_number_t   number_v;
        char           *string_v;  /* malloc，所有权归本节点 */
        json_array_t   *array_v;   /* malloc */
        json_object_t  *object_v;  /* malloc */
    } as;
};

/* ===== 构造（全部返回堆分配节点，失败返回 NULL）===== */
json_value_t *json_null_new(void);
json_value_t *json_bool_new(bool b);
json_value_t *json_int_new(long v);
json_value_t *json_double_new(double v);
json_value_t *json_string_new(const char *s);
json_value_t *json_array_new(void);
json_value_t *json_object_new(void);

/* ===== 容器操作 =====
 * push/put 转移 value 的所有权（存进去后不要再自己 free）*/
bool json_array_push(json_value_t *arr, json_value_t *item);
json_value_t *json_array_get(const json_value_t *arr, size_t i);
bool json_object_put(json_value_t *obj, const char *key, json_value_t *v);
json_value_t *json_object_get(const json_value_t *obj, const char *key);
bool json_object_has(const json_value_t *obj, const char *key);
size_t json_array_len(const json_value_t *arr);
size_t json_object_len(const json_value_t *obj);

/* ===== 递归释放整棵树 ===== */
void json_value_free(json_value_t *v);

/* 类型名（错误信息用）*/
const char *json_type_name(json_type_t t);

#endif
