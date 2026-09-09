#include "json_value.h"

#include <stdlib.h>
#include <string.h>

/* strdup 不在 C11 标准里，自己写一份（json_mapper.c 也 extern 引用）*/
char *xstrdup(const char *s);

char *xstrdup(const char *s) {
    size_t n = strlen(s) + 1;
    char *p = malloc(n);
    if (p) memcpy(p, s, n);
    return p;
}

static json_value_t *json_new(json_type_t t) {
    json_value_t *v = calloc(1, sizeof *v);
    if (!v) return NULL;
    v->type = t;
    return v;
}

json_value_t *json_null_new(void)      { return json_new(JSON_NULL); }

json_value_t *json_bool_new(bool b) {
    json_value_t *v = json_new(JSON_BOOL);
    if (v) v->as.bool_v = b;
    return v;
}

json_value_t *json_int_new(long x) {
    json_value_t *v = json_new(JSON_NUMBER);
    if (v) { v->as.number_v.is_int = true;  v->as.number_v.v.i = x; }
    return v;
}

json_value_t *json_double_new(double x) {
    json_value_t *v = json_new(JSON_NUMBER);
    if (v) { v->as.number_v.is_int = false; v->as.number_v.v.d = x; }
    return v;
}

json_value_t *json_string_new(const char *s) {
    json_value_t *v = json_new(JSON_STRING);
    if (!v) return NULL;
    v->as.string_v = xstrdup(s ? s : "");
    if (!v->as.string_v) { free(v); return NULL; }
    return v;
}

json_value_t *json_array_new(void) {
    json_value_t *v = json_new(JSON_ARRAY);
    if (!v) return NULL;
    v->as.array_v = calloc(1, sizeof *v->as.array_v);
    if (!v->as.array_v) { free(v); return NULL; }
    return v;
}

json_value_t *json_object_new(void) {
    json_value_t *v = json_new(JSON_OBJECT);
    if (!v) return NULL;
    v->as.object_v = calloc(1, sizeof *v->as.object_v);
    if (!v->as.object_v) { free(v); return NULL; }
    return v;
}

static bool arr_grow(json_array_t *a) {
    if (a->len < a->cap) return true;
    size_t nc = a->cap ? a->cap * 2 : 8;              /* ⭐ 翻倍扩容 */
    json_value_t **ni = realloc(a->items, nc * sizeof *ni);
    if (!ni) return false;
    a->items = ni;
    a->cap = nc;
    return true;
}

bool json_array_push(json_value_t *v, json_value_t *item) {
    if (!v || v->type != JSON_ARRAY || !item) return false;
    if (!arr_grow(v->as.array_v)) return false;
    v->as.array_v->items[v->as.array_v->len++] = item;
    return true;
}

json_value_t *json_array_get(const json_value_t *v, size_t i) {
    if (!v || v->type != JSON_ARRAY || i >= v->as.array_v->len) return NULL;
    return v->as.array_v->items[i];
}

size_t json_array_len(const json_value_t *v) {
    return (v && v->type == JSON_ARRAY) ? v->as.array_v->len : 0;
}

bool json_object_put(json_value_t *v, const char *key, json_value_t *val) {
    if (!v || v->type != JSON_OBJECT || !key || !val) return false;
    json_object_t *o = v->as.object_v;
    for (size_t i = 0; i < o->len; i++) {
        if (strcmp(o->keys[i], key) == 0) {           /* 同名 key → 覆盖并释放旧值 */
            json_value_free(o->values[i]);
            o->values[i] = val;
            return true;
        }
    }
    if (o->len == o->cap) {
        size_t nc = o->cap ? o->cap * 2 : 8;
        char **nk = realloc(o->keys, nc * sizeof *nk);
        if (!nk) return false;
        o->keys = nk;
        json_value_t **nv = realloc(o->values, nc * sizeof *nv);
        if (!nv) return false;
        o->values = nv;
        o->cap = nc;
    }
    o->keys[o->len] = xstrdup(key);
    o->values[o->len] = val;
    o->len++;
    return true;
}

json_value_t *json_object_get(const json_value_t *v, const char *key) {
    if (!v || v->type != JSON_OBJECT) return NULL;
    json_object_t *o = v->as.object_v;
    for (size_t i = 0; i < o->len; i++)
        if (strcmp(o->keys[i], key) == 0) return o->values[i];
    return NULL;
}

bool json_object_has(const json_value_t *v, const char *key) {
    return json_object_get(v, key) != NULL;
}

size_t json_object_len(const json_value_t *v) {
    return (v && v->type == JSON_OBJECT) ? v->as.object_v->len : 0;
}

const char *json_type_name(json_type_t t) {
    switch (t) {
    case JSON_NULL:   return "null";
    case JSON_BOOL:   return "bool";
    case JSON_NUMBER: return "number";
    case JSON_STRING: return "string";
    case JSON_ARRAY:  return "array";
    case JSON_OBJECT: return "object";
    }
    return "unknown";
}

/* ===== 递归释放整棵树（树所有权：谁的孩子谁负责）===== */
void json_value_free(json_value_t *v) {
    if (!v) return;
    switch (v->type) {
    case JSON_STRING:
        free(v->as.string_v);
        break;
    case JSON_ARRAY: {
        json_array_t *a = v->as.array_v;
        if (a) {
            for (size_t i = 0; i < a->len; i++)
                json_value_free(a->items[i]);     /* ⭐ 递归释放子树 */
            free(a->items);
            free(a);
        }
        break;
    }
    case JSON_OBJECT: {
        json_object_t *o = v->as.object_v;
        if (o) {
            for (size_t i = 0; i < o->len; i++) {
                json_value_free(o->values[i]);    /* 递归释放值 */
                free(o->keys[i]);                 /* key 也是 malloc 的 */
            }
            free(o->keys);
            free(o->values);
            free(o);
        }
        break;
    }
    case JSON_NULL:
    case JSON_BOOL:
    case JSON_NUMBER:
        break;                                    /* 无堆内存 */
    }
    free(v);                                      /* 最后释放节点自己 */
}
