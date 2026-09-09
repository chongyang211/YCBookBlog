/* Store（§06 + §10）：哈希表 + rwlock 的门面
 * 锁分类：写 set/del/expire/purge → wrlock；读 get/ttl/keys/size → rdlock
 * 铁律：store_get 锁内 clone 出参，锁外使用——绝不返回内部指针
 * （否则另一线程 DEL 同 key 后调用方摸到悬垂指针）*/
#ifndef MKV_STORE_H
#define MKV_STORE_H

#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "mkv/hashmap.h"
#include "mkv/value.h"

typedef struct mkv_store {
    pthread_rwlock_t rwlock;
    hashmap_t map;
} mkv_store_t;

void    store_init(mkv_store_t *s);
void    store_destroy(mkv_store_t *s);
bool    store_set(mkv_store_t *s, const char *key, mkv_value_t v);
bool    store_get(mkv_store_t *s, const char *key, mkv_value_t *out);
int     store_del(mkv_store_t *s, const char *key);
bool    store_expire(mkv_store_t *s, const char *key, int seconds);
int64_t store_ttl(mkv_store_t *s, const char *key);    /* -2 无 key，-1 永久 */
size_t  store_keys(mkv_store_t *s, char ***out_keys);  /* 调用方逐个 free */
size_t  store_size(mkv_store_t *s);
void    store_purge_expired(mkv_store_t *s);

#endif
