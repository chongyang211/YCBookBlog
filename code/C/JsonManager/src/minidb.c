#include "minidb.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "json_error.h"
#include "json_mapper.h"
#include "json_parser.h"

void minidb_init(minidb_t *db, const char *dir) {
    memset(db, 0, sizeof *db);
    db->dir = dir;
}

repository_t *minidb_register(minidb_t *db, const char *name,
                              const struct_desc_t *desc, const char *id_field) {
    if (db->count >= DB_MAX_TABLES) return NULL;
    for (size_t i = 0; i < db->count; i++)
        if (strcmp(db->tables[i].name, name) == 0) return NULL;   /* 已存在 */
    db_table_t *t = &db->tables[db->count];
    snprintf(t->name, sizeof t->name, "%s", name);
    if (!repo_init(&t->repo, desc, id_field)) return NULL;
    db->count++;
    return &t->repo;
}

repository_t *minidb_table(minidb_t *db, const char *name) {
    for (size_t i = 0; i < db->count; i++)
        if (strcmp(db->tables[i].name, name) == 0)
            return &db->tables[i].repo;
    return NULL;
}

/* ===== 文件 IO 小工具 ===== */

static char *read_file_all(const char *path) {      /* 返回 malloc 全文 */
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long n = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (n < 0) { fclose(f); return NULL; }
    char *buf = malloc((size_t)n + 1);
    if (!buf) { fclose(f); return NULL; }
    size_t rd = fread(buf, 1, (size_t)n, f);
    fclose(f);
    buf[rd] = '\0';
    return buf;
}

static bool write_file_all(const char *path, const char *content) {
    FILE *f = fopen(path, "wb");
    if (!f) return false;
    size_t n = strlen(content);
    bool ok = fwrite(content, 1, n, f) == n;
    fclose(f);
    return ok;
}

int minidb_load_all(minidb_t *db) {
    int loaded = 0;
    for (size_t i = 0; i < db->count; i++) {
        char path[256];
        snprintf(path, sizeof path, "%s/%s.json", db->dir, db->tables[i].name);
        char *text = read_file_all(path);
        if (!text) continue;                        /* 没有文件 = 空表，正常 */

        json_error_t err;
        json_value_t *jv = json_parse(text, &err);
        free(text);
        if (!jv || jv->type != JSON_ARRAY) {
            fprintf(stderr, "[minidb] 加载 %s 失败: %s\n", path,
                    jv ? "顶层不是数组" : err.msg);
            json_value_free(jv);
            continue;
        }
        bool ok = true;
        for (size_t k = 0; k < json_array_len(jv); k++) {
            void *entity = struct_new(db->tables[i].repo.desc);
            json_err_t rc = json_to_struct(entity, db->tables[i].repo.desc,
                                           json_array_get(jv, k), &err);
            if (rc != JSON_OK) {
                fprintf(stderr, "[minidb] %s 第 %zu 条: %s\n",
                        path, k + 1, err.msg);
                struct_free(entity, db->tables[i].repo.desc);
                ok = false;
                break;                              /* ⭐ fail-fast：一条坏全表拒载 */
            }
            repo_save(&db->tables[i].repo, entity);
            struct_free(entity, db->tables[i].repo.desc);   /* save 已深拷贝 */
        }
        json_value_free(jv);
        if (ok) loaded++;
    }
    return loaded;
}

bool minidb_save_all(minidb_t *db) {
    for (size_t i = 0; i < db->count; i++) {
        repository_t *r = &db->tables[i].repo;
        strbuf_t out;
        strbuf_init(&out);
        strbuf_append_ch(&out, '[');
        for (size_t k = 0; k < repo_count(r); k++) {
            if (k) strbuf_append_ch(&out, ',');
            struct_to_json(&out, repo_at(r, k), repo_desc(r));
        }
        strbuf_append_ch(&out, ']');

        char path[256];
        snprintf(path, sizeof path, "%s/%s.json", db->dir, db->tables[i].name);
        bool ok = write_file_all(path, out.data ? out.data : "[]");
        if (!ok) fprintf(stderr, "[minidb] 保存 %s 失败\n", path);
        strbuf_free(&out);
        if (!ok) return false;
    }
    return true;
}

void minidb_free(minidb_t *db) {
    for (size_t i = 0; i < db->count; i++)
        repo_free(&db->tables[i].repo);
    db->count = 0;
}
