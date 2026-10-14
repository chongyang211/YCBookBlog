//
// Created by 杨充 on 2026/6/10.
//
// ============================================================================
// 【文件定位】KvDatabase.h —— 基于 JSON 的内存 KV 数据库（项目顶层的"应用壳"）
// ----------------------------------------------------------------------------
// 【设计思路】组合前面三件套：
//   存储 = JsonNode 树（root 永远是 object）；
//   读入 = JsonParser（load 时文本→树）；
//   写出 = JsonWriter（save/get 时树→文本）。
//   对外只暴露 get/set/del 三个语义化操作 + load/save 持久化 ——
//   "解析器怎么写、Writer 怎么缩进"等实现细节对调用方完全不可见（分层封装）。
// 【脏标记设计】dirty 标记"改了没存"，析构时自动补存 —— 用户忘了 save
//   也不丢数据（Redis 风格的自动落盘思路）。
// ============================================================================

#ifndef KVDATABASE_H     // 头文件保护宏
#define KVDATABASE_H

#include "JsonNode.h"    // 存储载体：JsonNode / JsonNodePtr
#include <string>        // std::string：路径与文件名
#include <optional>      // std::optional：表达"可能没有"（C++17）

namespace jsonkv {

// 基于 JSON 的内存数据库：顶层是一个 object，支持 a.b.c 多级路径
class KvDatabase {
private:
    JsonNodePtr root;          // 顶层永远是 object
    // 【不变量】root->isObject() 恒成立：load 兜底 + set 只在 object 下钻 ——
    //   违反此不变量的外部输入（顶层是数组的 JSON 文件）会被 load 重置。
    std::string dataFile;      // 持久化文件路径
    bool dirty = false;        // 有未保存的修改？
    // 【生命周期】set/del 改树 → dirty=true；save/load 落盘/重载 → dirty=false；
    //   析构时若 dirty 则最后救一把（见析构函数注释）。

public:
    // explicit：单参构造（含默认值）防隐式转换；按值收 string 再 move 入成员
    explicit KvDatabase(std::string file = "kv.json");
    ~KvDatabase();     // 非默认析构：可能要在里面自动 save（不能 = default）

    void load();               // 从文件加载（文件不存在则空库）
    void save();               // 落盘保存

    // ⭐ optional 表达"可能没有"：找不到返回 nullopt
    // 【设计对比】三种"可能没有"的表达：
    //   ① 返回 JsonNode* + nullptr —— 裸指针，调用方无法区分"空值"和"没有"；
    //   ② 抛 KeyNotFound 异常 —— "查询失败"是常态而非错误，异常太重；
    //   ③ optional —— 值存在与否显式进入类型系统，调用方必须处理两种情况。
    //   这里 get 返回 string（序列化后的 JSON 片段）而非 JsonNode 引用：
    //   引用会暴露内部树结构、还有悬垂风险；字符串是值的快照，安全自足。
    std::optional<std::string> get(const std::string& path) const;
    void set(const std::string& path, JsonNodePtr value);   // 接管节点所有权（unique_ptr 移交）
    bool del(const std::string& path);                      // 返回是否真的删了

    // 顶层键值对数量（size 语义 = root object 的键数）
    size_t size() const { return root->size(); }
};

}

#endif //KVDATABASE_H    // 头文件保护宏结束
