#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include "json_error.h"
#include "json_value.h"

/* 解析整个 JSON 字符串。成功返回树根（调用方负责 json_value_free），
 * 失败返回 NULL，err（可传 NULL）里带行列号与人读信息 */
json_value_t *json_parse(const char *src, json_error_t *err);

#endif
