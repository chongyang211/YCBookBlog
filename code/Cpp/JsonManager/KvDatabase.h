//
// Created by 杨充 on 2026/6/10.
//

#ifndef KVDATABASE_H
#define KVDATABASE_H

#include "JsonNode.h"
#include <string>
#include <optional>

namespace jsonkv {

// 基于 JSON 的内存数据库：顶层是一个 object，支持 a.b.c 多级路径
class KvDatabase {
private:
    JsonNodePtr root;          // 顶层永远是 object
    std::string dataFile;      // 持久化文件路径
    bool dirty = false;        // 有未保存的修改？

public:
    explicit KvDatabase(std::string file = "kv.json");
    ~KvDatabase();

    void load();               // 从文件加载（文件不存在则空库）
    void save();               // 落盘保存

    // ⭐ optional 表达"可能没有"：找不到返回 nullopt
    std::optional<std::string> get(const std::string& path) const;
    void set(const std::string& path, JsonNodePtr value);
    bool del(const std::string& path);

    size_t size() const { return root->size(); }
};

}

#endif //KVDATABASE_H
