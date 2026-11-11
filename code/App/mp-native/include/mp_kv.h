// mp_kv.h —— K-V 存储（零依赖 · 全平台 · 单进程）
//
// 设计：bitcask 简化版 —— append-only 日志文件（dir/kv.log）+ 内存哈希索引。
//   - 写入：追加一条记录并 fflush（进程崩溃最多丢最后一条）
//   - 启动：重放日志重建索引（遇到截断的尾部记录自动停止——崩溃安全）
//   - 删除：追加墓碑记录；compact() 重写日志消除墓碑与旧值
//
// 线程模型：同一 db 句柄的读写内部加锁（可多线程共用）；不支持多进程同时
// 打开同一目录（会返回 MP_ERR_STATE）。
//
// 路径规则：dir_path 由宿主注入（Android 用 context.getFilesDir()/xxx，
// iOS 用沙盒 Documents，桌面用 userData 目录）——库永远不自己猜路径。
#ifndef MP_KV_H
#define MP_KV_H

#include "mp_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/** 不透明句柄（内部布局对 FFI 消费方不可见） */
typedef struct mp_kv mp_kv;

/**
 * 打开（或创建）一个 K-V 库。
 * @param dir_path 数据目录（不存在则自动创建，库文件为 dir/kv.log）
 * @param out_db   成功时 *out_db 为新句柄
 * @return MP_OK / MP_ERR_ARGS / MP_ERR_IO / MP_ERR_STATE（目录已被本进程打开）
 */
MP_API int32_t mp_kv_open(const char* dir_path, mp_kv** out_db);

/** 关闭并释放句柄（内部 flush）。句柄之后不可再用。 */
MP_API int32_t mp_kv_close(mp_kv* db);

/**
 * 写入（新增或覆盖）。二进制安全：value 可含 \0。
 * @param value_len 0 表示空值（value 可为 NULL）
 * @return MP_OK / MP_ERR_ARGS / MP_ERR_IO
 */
MP_API int32_t mp_kv_put(mp_kv* db, const char* key,
                         const void* value, size_t value_len);

/**
 * 读取。*out_value 指向本库分配的堆内存（即使 value_len==0 也非 NULL），
 * 用完必须 mp_free() 释放。
 * @return MP_OK / MP_ERR_NOT_FOUND / MP_ERR_ARGS
 */
MP_API int32_t mp_kv_get(mp_kv* db, const char* key,
                         void** out_value, size_t* out_len);

/** 便利接口：按 C 字符串读取（自动补 \0；值内含 \0 时会被截断）。mp_free 释放。 */
MP_API int32_t mp_kv_get_str(mp_kv* db, const char* key, char** out_value);

/** 删除。@return MP_OK（已删）/ MP_ERR_NOT_FOUND（本就不存在，不写日志） */
MP_API int32_t mp_kv_delete(mp_kv* db, const char* key);

/** @param out_exists 1 = 存在，0 = 不存在 */
MP_API int32_t mp_kv_has(mp_kv* db, const char* key, int32_t* out_exists);

/** 当前有效 key 数量 */
MP_API int64_t mp_kv_count(mp_kv* db);

/**
 * 遍历全部键值（快照语义：回调期间持有快照，宿主在回调里写库不会死锁/迭代失效）。
 * 回调返回非 0 时提前停止，该值作为 mp_kv_foreach 的返回值。
 */
typedef int32_t (*mp_kv_foreach_cb)(const char* key,
                                    const void* value, size_t value_len,
                                    void* user_data);
MP_API int32_t mp_kv_foreach(mp_kv* db, mp_kv_foreach_cb cb, void* user_data);

/**
 * 压实：重写日志，丢弃墓碑与被覆盖的旧值。
 * 删除/覆盖较多后调用可回收磁盘空间（打开时也会做一次尾部清理）。
 */
MP_API int32_t mp_kv_compact(mp_kv* db);

#ifdef __cplusplus
}
#endif
#endif  /* MP_KV_H */
