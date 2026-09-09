#ifndef JSON_WRITER_H
#define JSON_WRITER_H

#include <stdbool.h>

#include "json_value.h"
#include "strbuf.h"

/* compact：pretty=false；pretty：true（2 空格缩进）
 * 返回 malloc 字符串，调用方 free（失败返回 NULL）*/
char *json_write(const json_value_t *v, bool pretty);

/* 字符串按 JSON 标准转义后追加到 sb —— 导出给 json_mapper 复用
 * （教程挑战 B②：修复实体字段含引号/换行时输出非法 JSON 的缺陷）
 * 引号/反斜杠/控制字符走 \x 转义，控制字符输出 \u00XX，UTF-8 多字节透传 */
void json_escape_write(strbuf_t *sb, const char *s);

#endif
