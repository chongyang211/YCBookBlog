#include "repository.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "json_mapper.h"

bool repo_init(repository_t *r, const struct_desc_t *desc,
               const char *id_field) {
    memset(r, 0, sizeof *r);
    r->desc = desc;
    for (size_t i = 0; i < desc->field_count; i++) {
        if (strcmp(desc->fields[i].field_name, id_field) == 0) {
            if (desc->fields[i].type != FD_STRING) {
                fprintf(stderr, "repo_init: id 字段 %s 必须是 FD_STRING\n",
                        id_field);
                return false;
            }
            r->id_fd = &desc->fields[i];
            break;
        }
    }
    if (!r->id_fd) {
        fprintf(stderr, "repo_init: %s 没有 id 字段 %s\n",
                desc->struct_name, id_field);
        return false;
    }
    return true;
}

/* Java 的 Field.get(entity) → offset 取主键 */
static const char *entity_id(const repository_t *r, const void *e) {
    return *(char *const *)((const char *)e + r->id_fd->offset);
}

static bool repo_grow(repository_t *r) {
    if (r->len < r->cap) return true;
    size_t nc = r->cap ? r->cap * 2 : 8;
    void **ni = realloc(r->items, nc * sizeof *ni);
    if (!ni) return false;
    r->items = ni;
    r->cap = nc;
    return true;
}

bool repo_save(repository_t *r, const void *entity) {
    if (!entity) return false;
    const char *id = entity_id(r, entity);
    if (!id || !*id) return false;               /* 主键不能为空 */

    for (size_t i = 0; i < r->len; i++) {        /* 同 id → 覆盖 */
        if (strcmp(entity_id(r, r->items[i]), id) == 0) {
            struct_free(r->items[i], r->desc);   /* 释放旧实体 */
            r->items[i] = struct_clone(entity, r->desc);   /* ⭐ 深拷贝 */
            return r->items[i] != NULL;
        }
    }
    if (!repo_grow(r)) return false;
    void *copy = struct_clone(entity, r->desc);  /* ⭐ 深拷贝换隔离 */
    if (!copy) return false;
    r->items[r->len++] = copy;
    return true;
}

void *repo_find(const repository_t *r, const char *id) {
    for (size_t i = 0; i < r->len; i++)
        if (strcmp(entity_id(r, r->items[i]), id) == 0)
            return r->items[i];                  /* 借用：只读，勿 free */
    return NULL;
}

bool repo_delete(repository_t *r, const char *id) {
    for (size_t i = 0; i < r->len; i++) {
        if (strcmp(entity_id(r, r->items[i]), id) == 0) {
            struct_free(r->items[i], r->desc);
            memmove(&r->items[i], &r->items[i + 1],
                    (r->len - i - 1) * sizeof *r->items);
            r->len--;
            return true;
        }
    }
    return false;
}

void repo_free(repository_t *r) {
    for (size_t i = 0; i < r->len; i++)
        struct_free(r->items[i], r->desc);
    free(r->items);
    memset(r, 0, sizeof *r);
}

size_t repo_count(const repository_t *r) { return r->len; }
void *repo_at(const repository_t *r, size_t i) {
    return i < r->len ? r->items[i] : NULL;
}
const struct_desc_t *repo_desc(const repository_t *r) { return r->desc; }
