/* Store（§06 定义 + §10 加锁）：读写分类锁的门面 */
#include "mkv/store.h"

#include <stdlib.h>
#include <string.h>

char *mkv_xstrdup(const char *s);          /* value.c 提供 */

void store_init(mkv_store_t *s) {
    pthread_rwlock_init(&s->rwlock, NULL);
    hm_init(&s->map);
}

void store_destroy(mkv_store_t *s) {
    hm_destroy(&s->map);
    pthread_rwlock_destroy(&s->rwlock);
}

bool store_set(mkv_store_t *s, const char *key, mkv_value_t v) {
    pthread_rwlock_wrlock(&s->rwlock);             /* 写：独占锁 */
    hm_node_t *n = hm_find(&s->map, key);
    bool ok;
    if (n) {                                       /* 覆盖：版本+1 + 清 TTL */
        mkv_value_free(&n->entry.value);
        n->entry.value = v;                        /* 所有权转移进 map */
        n->entry.version++;
        n->entry.has_expire = false;               /* Redis 语义：SET 清旧 TTL */
        ok = true;
    } else {
        mkv_entry_t e = { .value = v, .created_ms = mkv_now_ms() };
        ok = hm_put(&s->map, key, e);
    }
    pthread_rwlock_unlock(&s->rwlock);
    return ok;
}

bool store_get(mkv_store_t *s, const char *key, mkv_value_t *out) {
    pthread_rwlock_rdlock(&s->rwlock);             /* ⭐ 读：共享锁 */
    hm_node_t *n = hm_find(&s->map, key);
    bool ok = n && !mkv_entry_expired(&n->entry);
    if (ok) *out = mkv_value_clone(&n->entry.value);   /* ⭐ 锁内 clone 出参 */
    pthread_rwlock_unlock(&s->rwlock);
    return ok;                                     /* 惰性删除：过期视同 miss */
}

int store_del(mkv_store_t *s, const char *key) {
    pthread_rwlock_wrlock(&s->rwlock);
    int deleted = hm_del(&s->map, key) ? 1 : 0;    /* 幂等：删不存在返 0 */
    pthread_rwlock_unlock(&s->rwlock);
    return deleted;
}

bool store_expire(mkv_store_t *s, const char *key, int seconds) {
    pthread_rwlock_wrlock(&s->rwlock);
    hm_node_t *n = hm_find(&s->map, key);
    bool ok = false;
    if (n) {
        n->entry.has_expire = true;
        n->entry.expire_ms = mkv_now_ms() + (int64_t)seconds * 1000;
        ok = true;
    }
    pthread_rwlock_unlock(&s->rwlock);
    return ok;
}

int64_t store_ttl(mkv_store_t *s, const char *key) {
    pthread_rwlock_rdlock(&s->rwlock);
    hm_node_t *n = hm_find(&s->map, key);
    int64_t ttl = -2;
    if (n) {
        if (!n->entry.has_expire) ttl = -1;
        else {
            int64_t left = (n->entry.expire_ms - mkv_now_ms()) / 1000;
            ttl = left < 0 ? 0 : left;
        }
    }
    pthread_rwlock_unlock(&s->rwlock);
    return ttl;
}

size_t store_keys(mkv_store_t *s, char ***out_keys) {
    pthread_rwlock_rdlock(&s->rwlock);
    char **keys = malloc((s->map.len ? s->map.len : 1) * sizeof *keys);
    size_t n = 0;
    if (keys) {
        for (size_t i = 0; i < s->map.cap; i++)
            for (hm_node_t *node = s->map.buckets[i]; node; node = node->next)
                if (!mkv_entry_expired(&node->entry))   /* 过期的不列出 */
                    keys[n++] = mkv_xstrdup(node->key);
    }
    pthread_rwlock_unlock(&s->rwlock);
    *out_keys = keys;
    return n;                                      /* 调用方逐个 free */
}

size_t store_size(mkv_store_t *s) {
    pthread_rwlock_rdlock(&s->rwlock);
    size_t n = hm_size(&s->map);
    pthread_rwlock_unlock(&s->rwlock);
    return n;
}

/* 主动清理（后台线程周期调用）：遍历全部桶删过期节点 */
void store_purge_expired(mkv_store_t *s) {
    pthread_rwlock_wrlock(&s->rwlock);
    for (size_t i = 0; i < s->map.cap; i++) {
        hm_node_t *n = s->map.buckets[i];
        while (n) {
            hm_node_t *next = n->next;          /* ⭐ 先存 next 再删（防线）*/
            if (mkv_entry_expired(&n->entry)) hm_del(&s->map, n->key);
            n = next;
        }
    }
    pthread_rwlock_unlock(&s->rwlock);
}
