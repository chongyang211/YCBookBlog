/* 手写哈希表（§06）：FNV-1a + 链地址法 + 负载因子 ≥2 翻倍 rehash */
#include "mkv/hashmap.h"

#include <stdlib.h>
#include <string.h>

char *mkv_xstrdup(const char *s);          /* value.c 提供 */

/* ===== FNV-1a：最流行的教学级字符串哈希 ===== */
static size_t fnv1a(const char *s) {
    size_t h = 14695981039346656037ULL;     /* offset basis */
    while (*s) {
        h ^= (unsigned char)*s++;
        h *= 1099511628211ULL;              /* prime */
    }
    return h;
}

bool hm_init(hashmap_t *m) {
    m->cap = 16;                            /* 初始 16 桶（2 的幂）*/
    m->len = 0;
    m->buckets = calloc(m->cap, sizeof *m->buckets);
    return m->buckets != NULL;
}

void hm_destroy(hashmap_t *m) {
    for (size_t i = 0; i < m->cap; i++) {
        hm_node_t *n = m->buckets[i];
        while (n) {
            hm_node_t *next = n->next;
            free(n->key);
            mkv_value_free(&n->entry.value);
            free(n);
            n = next;
        }
    }
    free(m->buckets);
    m->buckets = NULL;
    m->len = m->cap = 0;
}

hm_node_t *hm_find(hashmap_t *m, const char *key) {
    for (hm_node_t *n = m->buckets[fnv1a(key) & (m->cap - 1)]; n; n = n->next)
        if (strcmp(n->key, key) == 0) return n;
    return NULL;
}

/* 负载因子 ≥ 2 时翻倍 rehash */
static bool hm_grow(hashmap_t *m) {
    if (m->len < m->cap * 2) return true;
    size_t ncap = m->cap * 2;
    hm_node_t **nb = calloc(ncap, sizeof *nb);
    if (!nb) return false;
    for (size_t i = 0; i < m->cap; i++) {       /* 旧桶逐节点搬新家 */
        hm_node_t *n = m->buckets[i];
        while (n) {
            hm_node_t *next = n->next;
            size_t h = fnv1a(n->key) & (ncap - 1);
            n->next = nb[h];                    /* 头插进新桶 */
            nb[h] = n;
            n = next;
        }
    }
    free(m->buckets);
    m->buckets = nb;
    m->cap = ncap;
    return true;
}

bool hm_put(hashmap_t *m, const char *key, mkv_entry_t e) {
    if (!hm_grow(m)) return false;
    size_t h = fnv1a(key) & (m->cap - 1);
    for (hm_node_t *n = m->buckets[h]; n; n = n->next) {
        if (strcmp(n->key, key) == 0) {         /* 已存在 → 覆盖 */
            mkv_value_free(&n->entry.value);
            n->entry = e;                       /* value 所有权转移给节点 */
            return true;
        }
    }
    hm_node_t *n = malloc(sizeof *n);           /* 头插新节点 */
    if (!n) return false;
    n->key = mkv_xstrdup(key);
    n->entry = e;
    n->next = m->buckets[h];
    m->buckets[h] = n;
    m->len++;
    return true;
}

/* ⭐ 指针的指针删节点（§6.3）：pp 指向"存放节点地址的那个槽"
 * （桶头或某节点的 next 字段），*pp = dead->next 一步接链，
 * 头/中/尾零特判——Linux 内核 list_del 同款写法 */
bool hm_del(hashmap_t *m, const char *key) {
    size_t h = fnv1a(key) & (m->cap - 1);
    hm_node_t **pp = &m->buckets[h];
    while (*pp) {
        if (strcmp((*pp)->key, key) == 0) {
            hm_node_t *dead = *pp;
            *pp = dead->next;         /* ⭐ 一步接链 */
            free(dead->key);
            mkv_value_free(&dead->entry.value);
            free(dead);
            m->len--;
            return true;
        }
        pp = &(*pp)->next;
    }
    return false;
}

size_t hm_size(hashmap_t *m) { return m->len; }
