//
// Created by 杨充 on 2026/6/10.
//
// ============================================================================
// 【文件定位】KvDatabase.cpp —— 内存 KV 数据库实现（load/save/get/set/del）
// ----------------------------------------------------------------------------
// 【实现难点聚焦】set 的多级路径："a.b.c = v" 中途的 a、a.b 可能不存在
//   （要逐级创建）也可能是标量（类型冲突要拒绝）—— 本实现用"两遍走"：
//   第一遍只读校验路径合法性，第二遍才真正修改 —— 中途失败不留半成品（原子性）。
// ============================================================================

#include "KvDatabase.h"
#include "JsonParser.h"     // load：文本 → 树
#include "JsonWriter.h"     // save/get：树 → 文本
#include <fstream>          // ifstream/ofstream：文件流
#include <sstream>          // stringstream：整文件读入

namespace jsonkv {

// 构造：造一个空 object 当 root + 记住文件名
KvDatabase::KvDatabase(std::string file)
    : root(makeObject()), dataFile(std::move(file)) {}
// 【初始化顺序】root 与 dataFile 的初始化顺序由"类内声明顺序"决定，
//   与初始化列表书写顺序无关（乱写会有编译警告）—— 标准的强制约定。

// 析构：脏了就自动补存（用户的"最后防线"）
KvDatabase::~KvDatabase() {
    if (dirty) {
        try { save(); }
        catch (...) { /* 析构不能让异常逃出 */ }
        // 【铁律】C++ 析构函数中逃出的异常若在栈展开途中 = std::terminate
        //   直接崩溃 —— 所以析构里的 save 必须 try-catch 全部吞掉（文件
        //   写失败只能认栽，日志都未必打得出来）。这也是"析构不抛异常"
        //   这条工程铁律的现场教学。
    }
}

// 加载：整文件读入 → 解析 → 顶层必须是 object
void KvDatabase::load() {
    std::ifstream ifs(dataFile);
    if (!ifs) return;                       // 首次运行无文件，OK（空库继续，不算错误）
    std::stringstream ss;
    ss << ifs.rdbuf();                      // rdbuf 整体灌入流（比逐行读快且保留换行）
    std::string content = ss.str();         // ⭐ 先保存到局部变量，避免 string_view 悬垂
    // 【悬垂警示】若写成 JsonParser p(ss.str()); —— ss.str() 返回的临时 string
    //   在这条语句结束就析构，parser 里的 string_view 指向已释放内存（UB）！
    //   先存具名局部变量 content（活到 parse 结束），view 才安全 —— 这正是
    //   教程 9.3 节"string_view 危险"的实景防线。
    JsonParser p(content);
    root = p.parse();                       // 解析失败会抛 JsonParseError（交给调用方）
    if (!root->isObject()) {                // 兜底：顶层必须是 object
        root = makeObject();                // 顶层是数组/数字等：无法当 KV 库 → 重置空库
    }
    dirty = false;                          // 刚读入的内存与文件一致 → 不脏
}

// 保存：美化序列化 → 整体覆盖写
void KvDatabase::save() {
    JsonWriter w(2);                        // 美化输出（kv.json 人可读可编辑）
    std::ofstream ofs(dataFile);            // 覆盖式打开（旧文件直接被替换）
    if (!ofs) throw JsonIoError("cannot open " + dataFile);   // 打不开是错误（与 load 的宽容形成对比）
    ofs << w.write(*root);
    dirty = false;                          // 落盘成功 → 不脏
}

// 查询：路径取节点 → 紧凑序列化为字符串片段
std::optional<std::string> KvDatabase::get(const std::string& path) const {
    try {
        const JsonNode& node = root->at(path);      // at 沿 a.b.c 逐层下钻
        JsonWriter w(0);                     // 紧凑序列化（查询结果无缩进，省字节）
        return w.write(node);                // 有值：optional 装上字符串（隐式构造）
    } catch (const KeyNotFoundError&) {
        return std::nullopt;                 // ⭐ 找不到返回 nullopt
        // 【异常边界翻译】内部用异常（fail-fast 好实现），对外转 optional
        //   （查询 miss 是常态）—— "内部严格、外部友好"的接口分层。
        //   注意只接 KeyNotFound：TypeMismatch（路径撞上标量）继续往外抛 ——
        //   那是调用方的路径写错了，属于真正的错误。
    }
}

// 写入：多级路径逐级下钻（不存在就造 object），两遍走保证原子性
void KvDatabase::set(const std::string& path, JsonNodePtr value) {
    // ⭐ 第一遍：只走读路径，确保中间节点要么不存在要么是 object
    // 这样中途抛异常时不会留下"半成品"状态（保证原子性）
    // 【原子性动机】若一遍走（边检查边建 object），走到 a.b 发现它是
    //   number 抛异常 —— 但此刻 a.x、a.y 等兄弟节点已被误建出来，
    //   库里多出"半成品路径"。两遍走：校验全过才动第一个字节。
    JsonNode* check = root.get();       // 裸指针"巡视"用（只读不改，所有权仍在 root）
    size_t s = 0;
    while (true) {
        size_t d = path.find('.', s);           // 找下一段分隔点
        if (d == std::string::npos) break;      // 没有更多中间层：校验完毕
        std::string seg = path.substr(s, d - s);    // 取出这一段键
        if (!check->isObject()) throw TypeMismatchError("object", "non-object on path");
        // 中途某层是数组/标量：路径不可能走通 → 立即拒绝
        auto& obj = check->asObject();
        if (obj.find(seg) != obj.end() && !obj[seg]->isObject()) {
            throw TypeMismatchError("object", "non-object at " + seg);    // 键存在但是标量/数组
        }
        if (obj.find(seg) == obj.end()) break;   // 后面要建空 object，无需继续校验
        // 【推理】此键不存在 → 第二遍会重建为 object → 之后的路径全新无冲突
        check = obj[seg].get();          // 沿存在的键继续往下巡视
        s = d + 1;
    }

    // ⭐ 第二遍：真正修改（这一遍不会再抛 TypeMismatch）
    JsonNode* cur = root.get();
    size_t start = 0;
    while (true) {
        size_t dot = path.find('.', start);
        std::string seg = (dot == std::string::npos)
                          ? path.substr(start)
                          : path.substr(start, dot - start);    // 当前段键名

        if (dot == std::string::npos) {                    // 终点：直接赋值
            cur->asObject()[seg] = std::move(value);        // 节点所有权移入库树
            // map::operator[]：键不存在则插入、存在则覆盖 —— SET 的天然语义
            dirty = true;                                   // 改动发生 → 标脏
            return;
        }

        // 中间路径：不存在或不是 object 就建一个空 object
        auto& obj = cur->asObject();
        if (obj.find(seg) == obj.end() || !obj[seg]->isObject()) {
            obj[seg] = makeObject();            // 造空 object 垫脚（第二遍才允许建）
        }
        cur = obj[seg].get();                   // 继续下钻
        start = dot + 1;
    }
}

// 删除：单层直接删；多层先定位父节点再删末段键
bool KvDatabase::del(const std::string& path) {
    size_t dot = path.rfind('.');               // 从右往左找最后一个 '.'（末段前的分界）
    if (dot == std::string::npos) {
        // 单层：直接 erase
        bool ok = root->asObject().erase(path) > 0;     // erase 返回删除数量（0/1）
        if (ok) dirty = true;
        return ok;                               // 键不存在 → false（幂等：删两次第二次 false）
    }
    // 多层：先找到父节点
    try {
        JsonNode& parent = root->at(path.substr(0, dot));    // 前缀路径定位父 object
        if (!parent.isObject()) return false;                // 父不是 object：无从删起
        bool ok = parent.asObject().erase(path.substr(dot + 1)) > 0;    // 删末段键
        if (ok) dirty = true;
        return ok;
    } catch (const KeyNotFoundError&) {
        return false;                            // 前缀路径不存在：目标必不存在
    }
}

}
