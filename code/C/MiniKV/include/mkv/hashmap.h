/* 手写哈希表（§06）：FNV-1a + 链地址法 + 翻倍 rehash
 * C 没有容器的缺口在这里补上——unordered_map 的裸机版 */
#ifndef MKV_HASHMAP_H
#define MKV_HASHMAP_H

#include <stdbool.h>
#include <stddef.h>
#include "mkv/entry.h"

typedef struct hm_node {
    char *key;                  /* malloc 拷贝，所有权归节点 */
    mkv_entry_t entry;          /* value 所有权随节点 */
    struct hm_node *next;       /* 同桶冲突链 */
} hm_node_t;

typedef struct {
    hm_node_t **buckets;        /* 桶数组 */
    size_t cap;                 /* 桶数（恒为 2 的幂：h & (cap-1) 位与取模）*/
    size_t len;
} hashmap_t;

bool  hm_init(hashmap_t *m);
void  hm_destroy(hashmap_t *m);
hm_node_t *hm_find(hashmap_t *m, const char *key);
bool  hm_put(hashmap_t *m, const char *key, mkv_entry_t e);
bool  hm_del(hashmap_t *m, const char *key);
size_t hm_size(hashmap_t *m);

#endif
