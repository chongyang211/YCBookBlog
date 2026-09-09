#ifndef JSON_MAPPER_H
#define JSON_MAPPER_H

#include "json_error.h"
#include "json_schema.h"
#include "json_value.h"
#include "strbuf.h"

/* JSON 树 → 结构体（obj 必须是 calloc 的全新实例）*/
json_err_t json_to_struct(void *obj, const struct_desc_t *desc,
                          const json_value_t *jv, json_error_t *err);

/* 结构体 → compact JSON 字符串（strbuf 追加，调用方先 init）*/
json_err_t struct_to_json(strbuf_t *out, const void *obj,
                          const struct_desc_t *desc);

/* 三件套：new = calloc"无参构造"；free 递归释放；clone 深拷贝 */
void *struct_new(const struct_desc_t *desc);
void  struct_free(void *obj, const struct_desc_t *desc);
void *struct_clone(const void *obj, const struct_desc_t *desc);

#endif
