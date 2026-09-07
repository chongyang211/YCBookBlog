//
// Created by 杨充 on 2026/6/10.
//

#include "KvDatabase.h"
#include "JsonParser.h"
#include "JsonWriter.h"
#include <fstream>
#include <sstream>

namespace jsonkv {

KvDatabase::KvDatabase(std::string file)
    : root(makeObject()), dataFile(std::move(file)) {}

KvDatabase::~KvDatabase() {
    if (dirty) {
        try { save(); }
        catch (...) { /* 析构不能让异常逃出 */ }
    }
}

void KvDatabase::load() {
    std::ifstream ifs(dataFile);
    if (!ifs) return;                       // 首次运行无文件，OK
    std::stringstream ss;
    ss << ifs.rdbuf();
    std::string content = ss.str();         // ⭐ 先保存到局部变量，避免 string_view 悬垂
    JsonParser p(content);
    root = p.parse();
    if (!root->isObject()) {                // 兜底：顶层必须是 object
        root = makeObject();
    }
    dirty = false;
}

void KvDatabase::save() {
    JsonWriter w(2);                        // 美化输出
    std::ofstream ofs(dataFile);
    if (!ofs) throw JsonIoError("cannot open " + dataFile);
    ofs << w.write(*root);
    dirty = false;
}

std::optional<std::string> KvDatabase::get(const std::string& path) const {
    try {
        const JsonNode& node = root->at(path);
        JsonWriter w(0);                     // 紧凑序列化
        return w.write(node);
    } catch (const KeyNotFoundError&) {
        return std::nullopt;                 // ⭐ 找不到返回 nullopt
    }
}

void KvDatabase::set(const std::string& path, JsonNodePtr value) {
    // ⭐ 第一遍：只走读路径，确保中间节点要么不存在要么是 object
    // 这样中途抛异常时不会留下"半成品"状态（保证原子性）
    JsonNode* check = root.get();
    size_t s = 0;
    while (true) {
        size_t d = path.find('.', s);
        if (d == std::string::npos) break;
        std::string seg = path.substr(s, d - s);
        if (!check->isObject()) throw TypeMismatchError("object", "non-object on path");
        auto& obj = check->asObject();
        if (obj.find(seg) != obj.end() && !obj[seg]->isObject()) {
            throw TypeMismatchError("object", "non-object at " + seg);
        }
        if (obj.find(seg) == obj.end()) break;   // 后面要建空 object，无需继续校验
        check = obj[seg].get();
        s = d + 1;
    }

    // ⭐ 第二遍：真正修改（这一遍不会再抛 TypeMismatch）
    JsonNode* cur = root.get();
    size_t start = 0;
    while (true) {
        size_t dot = path.find('.', start);
        std::string seg = (dot == std::string::npos)
                          ? path.substr(start)
                          : path.substr(start, dot - start);

        if (dot == std::string::npos) {                    // 终点：直接赋值
            cur->asObject()[seg] = std::move(value);
            dirty = true;
            return;
        }

        // 中间路径：不存在或不是 object 就建一个空 object
        auto& obj = cur->asObject();
        if (obj.find(seg) == obj.end() || !obj[seg]->isObject()) {
            obj[seg] = makeObject();
        }
        cur = obj[seg].get();
        start = dot + 1;
    }
}

bool KvDatabase::del(const std::string& path) {
    size_t dot = path.rfind('.');
    if (dot == std::string::npos) {
        // 单层：直接 erase
        bool ok = root->asObject().erase(path) > 0;
        if (ok) dirty = true;
        return ok;
    }
    // 多层：先找到父节点
    try {
        JsonNode& parent = root->at(path.substr(0, dot));
        if (!parent.isObject()) return false;
        bool ok = parent.asObject().erase(path.substr(dot + 1)) > 0;
        if (ok) dirty = true;
        return ok;
    } catch (const KeyNotFoundError&) {
        return false;
    }
}

}
