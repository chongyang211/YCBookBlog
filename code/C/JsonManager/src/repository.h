#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <stdbool.h>
#include <stddef.h>

#include "json_schema.h"

/* C 的"泛型仓储"：void* + struct_desc_t（desc 就是 C 的 Class<T>）*/
typedef struct {
    const struct_desc_t *desc;
    const field_desc_t  *id_fd;    /* 主键字段（要求 FD_STRING）*/
    void               **items;    /* 实体指针数组（repo 深拷贝持有）*/
    size_t len, cap;
} repository_t;

/* id_field 必须是 desc 里 FD_STRING 类型的字段；失败返回 false */
bool repo_init(repository_t *r, const struct_desc_t *desc,
               const char *id_field);
void repo_free(repository_t *r);

/* 深拷贝入仓；同 id 覆盖（释放旧实体）*/
bool repo_save(repository_t *r, const void *entity);
void *repo_find(const repository_t *r, const char *id);   /* 借用：只读勿 free */
bool  repo_delete(repository_t *r, const char *id);
size_t repo_count(const repository_t *r);

/* 遍历用（REPL 查询需要）*/
void *repo_at(const repository_t *r, size_t i);
const struct_desc_t *repo_desc(const repository_t *r);

#endif
