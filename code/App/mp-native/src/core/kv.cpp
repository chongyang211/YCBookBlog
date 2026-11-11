// src/core/kv.cpp —— K-V 存储实现（bitcask 简化版）
//
// 数据格式（kv.log，二进制定长头 + 变长体，二进制安全）：
//   [op u8][klen u32LE][vlen u32LE][key bytes][value bytes]
//   op: 1 = PUT（vlen 可为 0）   2 = DELETE（墓碑，vlen 恒 0，无 value 体）
//
// 崩溃安全：写入前追加、fflush 落盘；重放遇到不完整尾部记录即停止（视为未提交）。
#include "mp_kv.h"

#include <cstdio>
#include <cstring>
#include <string>
#include <unordered_map>
#include <mutex>
#include <set>
#include <vector>
#include <algorithm>

#if defined(_WIN32)
  #include <direct.h>
  #define MP_MKDIR(p) _mkdir(p)
#else
  #include <sys/stat.h>
  #include <sys/types.h>
  #define MP_MKDIR(p) ::mkdir(p, 0755)
#endif

namespace {

constexpr uint32_t MP_KV_MAX_KEY = 1024;             // key 上限（字节）
constexpr uint64_t MP_KV_MAX_VAL = 64ull << 20;      // value 上限（64 MB）
constexpr char       WAL_NAME[]   = "kv.log";
constexpr char       TMP_NAME[]   = "kv.log.tmp";

constexpr uint8_t   OP_PUT    = 1;
constexpr uint8_t   OP_DELETE = 2;
constexpr size_t    HDR_SIZE  = 1 + 4 + 4;           // op + klen + vlen

struct KvStore {
    std::mutex mu;
    std::string dir;                                    // 数据目录
    std::FILE*  wal = nullptr;                          // 追加日志
    std::unordered_map<std::string, std::string> map;   // 内存索引（主存）
    size_t stale = 0;                                   // 被覆盖/删除的字节数（压实依据）
};

/* ---- 进程级打开登记：防止同一目录被打开两次导致日志交叉写 ---- */
std::mutex g_open_mu;
std::set<std::string> g_open_dirs;

void mkdirs(const std::string& path) {
    std::string cur;
    for (size_t i = 0; i < path.size(); ++i) {
        cur += path[i];
        if (path[i] == '/' || path[i] == '\\' || i + 1 == path.size()) {
            MP_MKDIR(cur.c_str());   // 已存在时返回 -1，忽略即可
        }
    }
}

void wr_u32(uint8_t* p, uint32_t v) {
    p[0] = static_cast<uint8_t>(v);
    p[1] = static_cast<uint8_t>(v >> 8);
    p[2] = static_cast<uint8_t>(v >> 16);
    p[3] = static_cast<uint8_t>(v >> 24);
}

uint32_t rd_u32(const uint8_t* p) {
    return static_cast<uint32_t>(p[0]) | (static_cast<uint32_t>(p[1]) << 8)
         | (static_cast<uint32_t>(p[2]) << 16) | (static_cast<uint32_t>(p[3]) << 24);
}

/* 追加一条记录并落盘。调用方持有锁。 */
int32_t append_record(std::FILE* f, uint8_t op,
                      const std::string& key, const std::string& value) {
    std::vector<uint8_t> buf(HDR_SIZE + key.size() +
                             (op == OP_PUT ? value.size() : 0));
    buf[0] = op;
    wr_u32(&buf[1], static_cast<uint32_t>(key.size()));
    wr_u32(&buf[5], static_cast<uint32_t>(op == OP_PUT ? value.size() : 0));
    std::memcpy(buf.data() + HDR_SIZE, key.data(), key.size());
    if (op == OP_PUT && !value.empty()) {
        std::memcpy(buf.data() + HDR_SIZE + key.size(), value.data(), value.size());
    }
    if (std::fwrite(buf.data(), 1, buf.size(), f) != buf.size()) return MP_ERR_IO;
    if (std::fflush(f) != 0) return MP_ERR_IO;
    return MP_OK;
}

/* 重放日志重建索引；遇到不完整尾部即停（崩溃安全）。 */
void replay(KvStore* db) {
    const std::string path = db->dir + "/" + WAL_NAME;
    std::FILE* r = std::fopen(path.c_str(), "rb");
    if (!r) return;

    std::fseek(r, 0, SEEK_END);
    const long size = std::ftell(r);
    std::fseek(r, 0, SEEK_SET);
    if (size <= 0) { std::fclose(r); return; }

    std::vector<uint8_t> data(static_cast<size_t>(size));
    const size_t got = std::fread(data.data(), 1, data.size(), r);
    std::fclose(r);
    if (got == 0) return;

    size_t off = 0;
    while (off + HDR_SIZE <= got) {
        const uint8_t  op   = data[off];
        const uint32_t klen = rd_u32(&data[off + 1]);
        const uint32_t vlen = rd_u32(&data[off + 5]);
        if ((op != OP_PUT && op != OP_DELETE) ||
            klen == 0 || klen > MP_KV_MAX_KEY ||
            off + HDR_SIZE + klen + vlen > got) {
            break;   // 坏记录 / 截断尾部 → 停止（compact 时清除）
        }
        std::string key(reinterpret_cast<const char*>(&data[off + HDR_SIZE]), klen);
        if (op == OP_PUT) {
            db->map[key].assign(reinterpret_cast<const char*>(
                &data[off + HDR_SIZE + klen]), vlen);
        } else {
            db->map.erase(key);
        }
        off += HDR_SIZE + klen + (op == OP_PUT ? vlen : 0);
    }
}

}  // namespace

/* ==================================================================== */
/* C API                                                                */
/* ==================================================================== */

extern "C" {

/* 边界转换：把不透明句柄还原为实现指针 */
namespace { KvStore* as_store(mp_kv* h) { return reinterpret_cast<KvStore*>(h); } }

int32_t mp_kv_open(const char* dir_path, mp_kv** out_db) {
    if (!dir_path || !*dir_path || !out_db) return MP_ERR_ARGS;
    *out_db = nullptr;

    mkdirs(dir_path);

    std::lock_guard<std::mutex> g(g_open_mu);
    if (g_open_dirs.count(dir_path)) return MP_ERR_STATE;

    KvStore* db = new (std::nothrow) KvStore();
    if (!db) return MP_ERR_OOM;
    db->dir = dir_path;

    db->wal = std::fopen((db->dir + "/" + WAL_NAME).c_str(), "ab");
    if (!db->wal) { delete db; return MP_ERR_IO; }

    replay(db);

    g_open_dirs.insert(dir_path);
    *out_db = reinterpret_cast<mp_kv*>(db);
    return MP_OK;
}

int32_t mp_kv_close(mp_kv* handle) {
    if (!handle) return MP_ERR_ARGS;
    KvStore* db = as_store(handle);
    {
        std::lock_guard<std::mutex> g(db->mu);
        if (db->wal) std::fclose(db->wal);
    }
    {
        std::lock_guard<std::mutex> g(g_open_mu);
        g_open_dirs.erase(db->dir);
    }
    delete db;
    return MP_OK;
}

int32_t mp_kv_put(mp_kv* handle, const char* key,
                  const void* value, size_t value_len) {
    if (!handle || !key || !*key) return MP_ERR_ARGS;
    KvStore* db = as_store(handle);
    const size_t klen = std::strlen(key);
    if (klen == 0 || klen > MP_KV_MAX_KEY) return MP_ERR_ARGS;
    if (value_len > MP_KV_MAX_VAL) return MP_ERR_ARGS;
    if (value_len > 0 && !value) return MP_ERR_ARGS;

    std::lock_guard<std::mutex> g(db->mu);
    const std::string k(key, klen);
    const int32_t rc = append_record(db->wal, OP_PUT, k,
        std::string(static_cast<const char*>(value), value_len));
    if (rc != MP_OK) return rc;

    auto it = db->map.find(k);
    if (it != db->map.end()) {
        db->stale += HDR_SIZE + klen + it->second.size();   // 旧记录变垃圾
        it->second.assign(static_cast<const char*>(value), value_len);
    } else {
        db->map.emplace(std::move(k),
            std::string(static_cast<const char*>(value), value_len));
    }
    return MP_OK;
}

int32_t mp_kv_get(mp_kv* handle, const char* key,
                  void** out_value, size_t* out_len) {
    if (!handle || !key || !out_value || !out_len) return MP_ERR_ARGS;
    KvStore* db = as_store(handle);
    *out_value = nullptr; *out_len = 0;

    std::lock_guard<std::mutex> g(db->mu);
    auto it = db->map.find(key);
    if (it == db->map.end()) return MP_ERR_NOT_FOUND;

    const size_t n = it->second.size();
    uint8_t* buf = static_cast<uint8_t*>(std::malloc(n ? n : 1));
    if (!buf) return MP_ERR_OOM;
    if (n) std::memcpy(buf, it->second.data(), n);
    *out_value = buf;
    *out_len = n;
    return MP_OK;
}

int32_t mp_kv_get_str(mp_kv* handle, const char* key, char** out_value) {
    void* raw = nullptr; size_t n = 0;
    const int32_t rc = mp_kv_get(handle, key, &raw, &n);
    if (rc != MP_OK) return rc;
    char* str = static_cast<char*>(std::realloc(raw, n + 1));
    if (!str) { std::free(raw); return MP_ERR_OOM; }
    str[n] = '\0';
    *out_value = str;
    return MP_OK;
}

int32_t mp_kv_delete(mp_kv* handle, const char* key) {
    if (!handle || !key || !*key) return MP_ERR_ARGS;
    KvStore* db = as_store(handle);

    std::lock_guard<std::mutex> g(db->mu);
    const std::string k(key);
    auto it = db->map.find(k);
    if (it == db->map.end()) return MP_ERR_NOT_FOUND;   // 不存在：不写墓碑

    const int32_t rc = append_record(db->wal, OP_DELETE, k, "");
    if (rc != MP_OK) return rc;
    db->stale += HDR_SIZE + k.size() + it->second.size();
    db->map.erase(it);
    return MP_OK;
}

int32_t mp_kv_has(mp_kv* handle, const char* key, int32_t* out_exists) {
    if (!handle || !key || !out_exists) return MP_ERR_ARGS;
    KvStore* db = as_store(handle);
    std::lock_guard<std::mutex> g(db->mu);
    *out_exists = db->map.count(key) ? 1 : 0;
    return MP_OK;
}

int64_t mp_kv_count(mp_kv* handle) {
    if (!handle) return 0;
    KvStore* db = as_store(handle);
    std::lock_guard<std::mutex> g(db->mu);
    return static_cast<int64_t>(db->map.size());
}

int32_t mp_kv_foreach(mp_kv* handle, mp_kv_foreach_cb cb, void* user_data) {
    if (!handle || !cb) return MP_ERR_ARGS;
    KvStore* db = as_store(handle);

    /* 快照语义：锁内拷贝，锁外回调 —— 宿主在回调里写库不会死锁 */
    std::vector<std::pair<std::string, std::string>> snap;
    {
        std::lock_guard<std::mutex> g(db->mu);
        snap.reserve(db->map.size());
        snap.assign(db->map.begin(), db->map.end());
    }
    for (const auto& kv : snap) {
        const int32_t rc = cb(kv.first.c_str(),
                              kv.second.data(), kv.second.size(), user_data);
        if (rc != MP_OK) return rc;   /* 回调要求提前停止 */
    }
    return MP_OK;
}

int32_t mp_kv_compact(mp_kv* handle) {
    if (!handle) return MP_ERR_ARGS;
    KvStore* db = as_store(handle);

    std::lock_guard<std::mutex> g(db->mu);
    const std::string log_path = db->dir + "/" + WAL_NAME;
    const std::string tmp_path = db->dir + "/" + TMP_NAME;

    /* 1. 重写有效数据到临时文件 */
    std::FILE* tmp = std::fopen(tmp_path.c_str(), "wb");
    if (!tmp) return MP_ERR_IO;
    for (const auto& kv : db->map) {
        if (append_record(tmp, OP_PUT, kv.first, kv.second) != MP_OK) {
            std::fclose(tmp);
            std::remove(tmp_path.c_str());
            return MP_ERR_IO;
        }
    }
    std::fclose(tmp);

    /* 2. 原子替换（Windows 下 rename 不覆盖，先删旧文件） */
    std::fclose(db->wal);
    std::remove(log_path.c_str());
    if (std::rename(tmp_path.c_str(), log_path.c_str()) != 0) {
        db->wal = std::fopen(log_path.c_str(), "ab");   // 尽力恢复
        return MP_ERR_IO;
    }
    db->wal = std::fopen(log_path.c_str(), "ab");
    if (!db->wal) return MP_ERR_IO;
    db->stale = 0;
    return MP_OK;
}

}  // extern "C"
