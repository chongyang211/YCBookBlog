#ifndef MINIDB_H
#define MINIDB_H

#include "repository.h"

#define DB_MAX_TABLES 16

typedef struct {
    char name[32];
    repository_t repo;
} db_table_t;

typedef struct {
    db_table_t tables[DB_MAX_TABLES];
    size_t count;
    const char *dir;                 /* data/ 目录 */
} minidb_t;

void minidb_init(minidb_t *db, const char *dir);
repository_t *minidb_register(minidb_t *db, const char *name,
                              const struct_desc_t *desc, const char *id_field);
repository_t *minidb_table(minidb_t *db, const char *name);

/* 加载/保存所有表到 <dir>/<name>.json */
int minidb_load_all(minidb_t *db);       /* 返回成功加载的表数 */
bool minidb_save_all(minidb_t *db);
void minidb_free(minidb_t *db);

#endif
