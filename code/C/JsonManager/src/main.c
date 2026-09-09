/* MiniDB REPL 入口：SELECT <table> WHERE <field> <op> <value> 简易查询
 * 对应教程《04.Json与内存数据库》§11 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>     /* strcasecmp / strncasecmp / strtok_r（POSIX）*/

#include "entity.h"
#include "json_mapper.h"
#include "json_parser.h"
#include "minidb.h"
#include "strbuf.h"

static void print_help(void) {
    printf("支持命令：\n"
           "  SHOW TABLES\n"
           "  SELECT <table>\n"
           "  SELECT <table> WHERE <field> <op> <value>\n"
           "    op: =  !=  >  >=  <  <=  ~（字符串包含）\n"
           "  INSERT student <id> <name> <age>     —— 演示用\n"
           "  DELETE <table> <id>\n"
           "  QUIT / EXIT\n");
}

/* 实体打印：C 没有运行时 toString —— 用描述表序列化成 JSON 行 */
static void print_entity(const void *entity, const struct_desc_t *desc) {
    strbuf_t out;
    strbuf_init(&out);
    struct_to_json(&out, entity, desc);
    printf("  %s\n", out.data ? out.data : "{}");
    strbuf_free(&out);
}

/* 匹配规则：数字字段走 strtod 比较，字符串字段走 strcmp/strstr
 * 字段比较复用描述表——field_desc_t 又立功了 */
static bool match_pred(const void *entity, const field_desc_t *fd,
                       const char *op, const char *literal) {
    const char *field = (const char *)entity + fd->offset;

    if (fd->type == FD_INT || fd->type == FD_LONG ||
        fd->type == FD_DOUBLE) {
        double cur = fd->type == FD_INT    ? *(const int *)field
                   : fd->type == FD_LONG   ? *(const long *)field
                                          : *(const double *)field;
        double target = strtod(literal, NULL);
        if      (strcmp(op, "=")  == 0) return cur == target;
        else if (strcmp(op, "!=") == 0) return cur != target;
        else if (strcmp(op, ">")  == 0) return cur >  target;
        else if (strcmp(op, ">=") == 0) return cur >= target;
        else if (strcmp(op, "<")  == 0) return cur <  target;
        else if (strcmp(op, "<=") == 0) return cur <= target;
        return false;
    }
    if (fd->type == FD_STRING) {
        const char *cur = *(char *const *)field;
        if (!cur) return false;
        if      (strcmp(op, "=")  == 0) return strcmp(cur, literal) == 0;
        else if (strcmp(op, "!=") == 0) return strcmp(cur, literal) != 0;
        else if (strcmp(op, "~")  == 0) return strstr(cur, literal) != NULL;
        return false;
    }
    return false;
}

static int do_select(minidb_t *db, char *rest) {
    /* rest = "<table> [WHERE <field> <op> <value>]" */
    char *save = NULL;
    char *table = strtok_r(rest, " \t", &save);
    if (!table) { printf("用法: SELECT <table> ...\n"); return 0; }

    repository_t *r = minidb_table(db, table);
    if (!r) { printf("✗ 表不存在: %s\n", table); return 0; }

    char *kw = strtok_r(NULL, " \t", &save);
    if (!kw) {                                     /* 全表 */
        for (size_t i = 0; i < repo_count(r); i++)
            print_entity(repo_at(r, i), repo_desc(r));
        return 0;
    }
    if (strcasecmp(kw, "WHERE") != 0) {
        printf("✗ 期望 WHERE，实际 %s\n", kw);
        return 0;
    }
    char *fname = strtok_r(NULL, " \t", &save);
    char *op    = strtok_r(NULL, " \t", &save);
    char *val   = strtok_r(NULL, " \t", &save);
    if (!fname || !op || !val) { printf("✗ WHERE 子句缺参数\n"); return 0; }

    /* 从描述表查字段（json_key 或 C 字段名都认）*/
    const field_desc_t *fd = NULL;
    for (size_t i = 0; i < repo_desc(r)->field_count; i++) {
        const field_desc_t *f = &repo_desc(r)->fields[i];
        if (strcmp(f->field_name, fname) == 0 ||
            strcmp(f->json_key,  fname) == 0) { fd = f; break; }
    }
    if (!fd) { printf("✗ %s 没有字段: %s\n",
                      repo_desc(r)->struct_name, fname); return 0; }

    for (size_t i = 0; i < repo_count(r); i++)
        if (match_pred(repo_at(r, i), fd, op, val))
            print_entity(repo_at(r, i), repo_desc(r));
    return 0;
}

int main(void) {
    printf("Mini JSON DB 启动 (C11)\n");
    minidb_t db;
    minidb_init(&db, "data");
    repository_t *stu = minidb_register(&db, "student", &STUDENT_DESC, "id");
    minidb_register(&db, "course",  &COURSE_DESC,  "code");
    minidb_load_all(&db);

    if (repo_count(stu) == 0) {                    /* 种子数据 */
        student_t seeds[] = {
            {"S001", "张三", 20}, {"S002", "李四", 21},
            {"S003", "王五", 19}, {"S004", "赵六", 22},
        };
        for (size_t i = 0; i < sizeof seeds / sizeof seeds[0]; i++)
            repo_save(stu, &seeds[i]);
        printf("[初始化] 种子数据加载完毕\n");
    }

    char line[512];
    printf("输入 HELP 查看命令，QUIT 退出\n");
    while (true) {
        printf("\n> ");
        if (!fgets(line, sizeof line, stdin)) break;
        line[strcspn(line, "\r\n")] = '\0';        /* 掐掉换行 */

        if (strcasecmp(line, "QUIT") == 0 || strcasecmp(line, "EXIT") == 0) {
            minidb_save_all(&db);
            printf("已保存。再见\n");
            break;
        }
        if (strcasecmp(line, "HELP") == 0) { print_help(); continue; }
        if (strcasecmp(line, "SHOW TABLES") == 0) {
            for (size_t i = 0; i < db.count; i++)
                printf(" - %s (%zu 行)\n", db.tables[i].name,
                       repo_count(&db.tables[i].repo));
            continue;
        }
        if (strncasecmp(line, "SELECT", 6) == 0) {
            do_select(&db, line + 6);
            continue;
        }
        if (strncasecmp(line, "INSERT", 6) == 0) {
            char id[64], name[64]; long age;
            if (sscanf(line + 6, " student %63s %63s %ld", id, name, &age) == 3) {
                student_t s = { id, name, (int)age };
                printf(repo_save(stu, &s) ? "✅ 已插入\n"
                                          : "✗ 插入失败\n");
            } else {
                printf("仅支持: INSERT student <id> <name> <age>\n");
            }
            continue;
        }
        if (strncasecmp(line, "DELETE", 6) == 0) {
            char tbl[64], id[64];
            if (sscanf(line + 6, " %63s %63s", tbl, id) == 2) {
                repository_t *r = minidb_table(&db, tbl);
                printf(r && repo_delete(r, id) ? "✅ 已删除\n"
                                              : "✗ 不存在\n");
            } else {
                printf("用法: DELETE <table> <id>\n");
            }
            continue;
        }
        printf("未知命令（HELP 查看用法）\n");
    }

    minidb_free(&db);
    return 0;
}
