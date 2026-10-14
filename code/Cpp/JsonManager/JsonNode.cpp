//
// Created by 杨充 on 2026/6/4.
//
// ============================================================================
// 【文件定位】JsonNode.cpp —— 节点访问器与容器操作实现
// ----------------------------------------------------------------------------
// 【实现套路】所有访问器都是"先校验类型，再 std::get 取值"两步：
//   校验失败抛自家的 TypeMismatchError（带期望/实际双侧信息），
//   而不是放任 std::get 抛消息干瘪的 bad_variant_access —— 异常体系的价值。
// ============================================================================

#include "JsonNode.h"


namespace jsonkv {

// 工具：枚举转字符串（用于错误消息）
// 【技术细节】static（内部链接）：仅本翻译单元可见，不污染链接命名空间；
//   switch 覆盖全部枚举值 + 末尾兜底 return，保证所有控制流路径都有返回。
static std::string typeName(JsonNode::Type t) {
    switch (t) {
        case JsonNode::Type::Null:   return "null";
        case JsonNode::Type::Bool:   return "bool";
        case JsonNode::Type::Number: return "number";
        case JsonNode::Type::String: return "string";
        case JsonNode::Type::Array:  return "array";
        case JsonNode::Type::Object: return "object";
    }
    return "unknown";
}


// ====== 访问器：校验 + std::get ======
// 【对比教学】"裸 std::get"版本（Step 2.4 的教学中间态）没有 if 校验，
//   取错类型时抛 std::bad_variant_access（消息只有 "bad variant access"，
//   无上下文）；升级版把校验前置 + 异常换成自家类型 —— 错误信息可用性质变。
bool JsonNode::asBool() const {
    if (!isBool()) throw TypeMismatchError("bool", typeName(type()));    // ⭐ 先校验
    return std::get<bool>(v);     // 校验已过，get 必然成功（编译期已知备选类型）
}

double JsonNode::asNumber() const {
    if (!isNumber()) throw TypeMismatchError("number", typeName(type()));
    return std::get<double>(v);
}

const std::string& JsonNode::asString() const {
    if (!isString()) throw TypeMismatchError("string", typeName(type()));
    return std::get<std::string>(v);
    // 【返回值设计】返回 const 引用而非 string 值：零拷贝暴露只读视图 ——
    //   调用方（如 Parser 取 key）只读不写，没必要复制一份。
}

const JsonNode::Array& JsonNode::asArray() const {
    if (!isArray()) throw TypeMismatchError("array", typeName(type()));
    return std::get<Array>(v);
}

const JsonNode::Object& JsonNode::asObject() const {
    if (!isObject()) throw TypeMismatchError("object", typeName(type()));
    return std::get<Object>(v);
}

// 可变版本：复用 const 版本的检查逻辑
// 【技术原理·const 复用三件套】这是 C++ 经典模式（Scott Meyers 条款 3）：
//   ① static_cast<const JsonNode*>(this) —— 给 this 加 const
//   ② 调 const 版（做校验 + 取值，返回 const 引用）
//   ③ const_cast<Array&> —— 再摘掉返回值上的 const
//   "偷 const"合法的前提：对象本身是非 const 的（从非 const 成员函数进来
//   必然如此）—— 校验逻辑只写一遍，两个版本永远同步。
JsonNode::Array&  JsonNode::asArray()  {
    return const_cast<Array&>(static_cast<const JsonNode*>(this)->asArray());
}
JsonNode::Object& JsonNode::asObject() {
    return const_cast<Object&>(static_cast<const JsonNode*>(this)->asObject());
}

// ====== operator[] ======
// 数组版：类型校验 + 越界检查，都通过才解引用子节点指针
JsonNode& JsonNode::operator[](size_t idx) {
    if (!isArray()) {
        throw TypeMismatchError("array", typeName(type()));    // 不是数组没有下标语义
    }
    auto& arr = std::get<Array>(v);       // 已校验，直接 get
    if (idx >= arr.size()) {
        throw std::out_of_range("array index out of range");   // 用标准异常表越界（语义恰合）
    }
    return *arr[idx];      // arr[idx] 是 unique_ptr，* 解引用得 JsonNode（返回引用长期有效）
}

// 对象版：类型校验 + 查 key
JsonNode& JsonNode::operator[](const std::string& key) {
    if (!isObject()) {
        throw TypeMismatchError("object", typeName(type()));    // 不是对象没有键语义
    }
    auto& obj = std::get<Object>(v);
    auto it = obj.find(key);             // map::find：O(logN)，不插入（与 operator[] 语义区分开）
    if (it == obj.end()) {
        // 方案A：抛异常
        throw KeyNotFoundError(key);
        // 方案B：自动插入（类似 nlohmann/json 的行为）
        // auto [newIt, _] = obj.emplace(key, makeNull());
        // return *newIt->second;
        // 【教学对比】方案B便利（写代码时 obj[k]=v 自动建键）但危险
        //   （拼错 key 的读操作会静默改数据结构）；教学版选 A：fail-fast。
    }
    return *it->second;
}

// ====== size：按类型分派（多态行为的 variant 版表达）======
size_t JsonNode::size() const {
    if (isArray())  return std::get<Array>(v).size();     // 数组：元素个数
    if (isObject()) return std::get<Object>(v).size();    // 对象：键值对个数
    if (isBool()) return 1;                               // bool：约定俗成算 1（Redis 同款语义）
    if (isString()) return std::get<std::string>(v).size();   // 字符串：字符数
    return 0;      // null/number：无"大小"概念，约定为 0（不抛错 —— 宽容查询）
}

// contains：只有 object 才可能"包含某个 key"
// 【短路设计】isObject() 为 false 直接返回 false，而非抛异常 ——
//   "询问"型接口（contains/size）宽容，"操作"型接口（at/operator[]）严格。
bool JsonNode::contains(const std::string& key) const {
    return isObject() && std::get<Object>(v).count(key) > 0;
}

// 路径解析：递归查找 a.b.c
// 【算法】"切下第一段 + 递归找剩下"：user.address.city
//   → 本层找 user → 子节点递归找 address.city → ... 直到无 '.' 剩余。
JsonNode& JsonNode::at(const std::string& path) {
    // 两个边界：空路径（查自己）或当前不是 object（无处下钻）都返回自身
    if (path.empty() || !isObject()) return *this;
    size_t dot = path.find('.');          // 第一个 '.' 的位置
    std::string head = (dot == std::string::npos) ? path : path.substr(0, dot);   // 第一段键
    std::string rest = (dot == std::string::npos) ? "" : path.substr(dot + 1);    // 剩余路径

    auto& obj = std::get<Object>(v);
    auto it = obj.find(head);
    if (it == obj.end()) throw KeyNotFoundError(head);    // 本层缺 key：立即报错（带缺的键名）
    if (rest.empty()) return *it->second;                 // 路径走完：返回目标节点
    return it->second->at(rest);             // ⭐ 递归：子节点接着找剩余路径
    // 【递归深度】= 路径层数，正常数据几层而已；不设防（恶意深层路径才会爆栈）
}

// const 版：复用非 const 版（正向的 const 复用 —— 与 asArray 的方向相反）
// 【合法性】const 对象调非 const at() 需要先 const_cast<this>：合法，因为
//   非 const 版对*this 的修改只发生在"它本来就是可变对象"时……严格说本处
//   依赖 at() 不真正改状态（只读路径查找），是教学简化（标准库的复用方向
//   是"非 const 调 const"，见 asArray —— 两处对照着看）。
const JsonNode& JsonNode::at(const std::string& path) const {
    return const_cast<JsonNode*>(this)->at(path);
}

}
