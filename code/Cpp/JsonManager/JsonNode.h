//
// Created by 杨充 on 2026/6/4.
//
// ============================================================================
// 【文件定位】JsonNode.h —— 核心：JSON 节点类（variant 实现"一节点六形态"）
// ----------------------------------------------------------------------------
// 【设计思想·为什么用 variant 而不是别的】
//   ① 对比"void* + type tag"（C 风格）：variant 类型安全 —— 取错类型直接
//     抛异常而非未定义行为；variant 自己管理析构，不会泄漏。
//   ② 对比"继承体系 NullNode/BoolNode/..."：无需堆分配、无虚表开销、
//     值语义天然可拷贝（树节点往往小而平凡，variant 更紧凑）。
//   ③ 代价：variant 只能装"大小不超过最大成员"的布局（这里 Object 最大，
//     节点统一胖一点）—— 用空间换类型安全，教学与工程都划算。
//   JSON 是"递归定义"的（数组里装值、值又可以是数组），所以 Array/Object
//   里存的是 unique_ptr<JsonNode>（间接层打破"类型含自身"的无限大小问题）。
// ============================================================================

#pragma once            // 头文件保护
#include <variant>      // std::variant：类型安全的标签联合（C++17）
#include <vector>       // Array 的底层数组
#include <map>          // Object 的底层红黑树（key 有序：输出稳定可 diff）
#include <string>       // 字符串值
#include <memory>       // std::unique_ptr / make_unique（独占所有权）
#include "JsonExceptions.h"    // 访问器抛 TypeMismatchError / KeyNotFoundError

namespace jsonkv {

// 前置声明
class JsonNode;
// 别名：节点智能指针
// 【设计要点】using 别名统一写法：全项目写 JsonNodePtr 而不是
//   std::unique_ptr<JsonNode> —— 将来若换 shared_ptr 只改这一行。
using JsonNodePtr = std::unique_ptr<JsonNode>;

class JsonNode {
public:
    // JSON 数组
    // 【技术原理】元素是 JsonNodePtr（而非 JsonNode 值）：variant 成员里再嵌
    //   自身类型会"无限大小"（vector<JsonNode> 要知道 JsonNode 大小才能分配，
    //   而 JsonNode 大小又取决于 vector...）—— 指针大小固定，打破递归依赖。
    using Array = std::vector<JsonNodePtr>;
    // JSON 对象
    // 【选型说明】map 而非 unordered_map：key 按字典序排列 —— Writer 输出的
    //   JSON 键序稳定，便于文件 diff 与回归测试（性能取舍：查找 O(logN) vs O(1)）。
    using Object = std::map<std::string, JsonNodePtr>;
    // ⭐ 核心：6 路 variant。6 种 JSON 类型
    // 使用 std::variant 替代传统 union 或继承体系
    // 【技术原理】variant = "标签 + 最大成员的存储空间 + 编译期生成的
    //   构造/析构/访问分派"：任一时刻恰好持有其中一种备选类型，
    //   index() 告诉你当前是哪一种；union 则要手工管标签、还要处理
    //   非平凡成员（string 不能进 union）—— variant 是 union 的现代化身。
    using Value  = std::variant<
        std::nullptr_t,    // 0: null
        bool,              // 1: true/false
        double,            // 2: number（JSON 不分整型/浮点，统一 double）
        std::string,       // 3: string
        Array,             // 4: array
        Object             // 5: object
    >;
    // 用 enum class 让"类型枚举"和 variant 索引一一对应
    // 【技术原理】variant 备选的顺序即索引（0..5）—— enum class 的枚举值
    //   显式对应索引，于是 type() 可以 O(1) 地用 index() 强转得到；
    //   enum class 的强作用域（必须写 Type::Array）防与其他枚举撞名。
    // variant 索引：       0       1     2       3       4       5
    enum class Type { Null = 0, Bool, Number, String, Array, Object };

    // ====== 构造函数：每种 JSON 类型一个 ======
    // 【对比说明】默认构造给 null（而非"空/非法"）—— JSON 语义里
    //   JsonNode() 就该是 null，与 JSON 字面量 null 对齐。
    JsonNode() : v(nullptr) {}                                   // null
    // explicit，禁止隐式转换。
    // 避免`JsonNode n = 3.14;` 会偷偷把 double 转成 JsonNode
    // 加上`explicit` 强制写 `JsonNode n{3.14}`
    // 【技术原理】explicit 关闭"单参构造的隐式转换通道"：
    //   防止 f(3.14) 在参数是 JsonNode 时被编译器静默转换成功（调用者
    //   完全没意识到造了个节点）；强制花括号显式表达意图。
    //   （连带效应：JsonNode n; n = 3.14; 也不能编译 —— 赋值要走
    //   拷贝/移动赋值，double 没法隐式变 JsonNode。main.cpp 的 test()
    //   里有一行故意写反的例子，就是为演示这一点。）
    explicit JsonNode(bool b)                : v(b) {}
    explicit JsonNode(double n)              : v(n) {}
    explicit JsonNode(int n)                 : v(static_cast<double>(n)) {}   // int 转 double
    // 【重载动机】int 单独重载：让 JsonNode{42} 直接可用（否则 42 是 int、
    //   variant 备选里没有 int，重载决议失败）—— 内部统一转 double 存储。
    explicit JsonNode(const std::string& s)  : v(s) {}
    explicit JsonNode(std::string&& s)       : v(std::move(s)) {}             // 移动版
    // 【移动重载动机】右值字符串直接"掏走"内部缓冲区（O(1)），比 const&
    //   版本深拷贝（O(n)）快 —— 解析器逐字符攒出的字符串正好能享受。
    explicit JsonNode(const char* s)         : v(std::string(s)) {}           // 字面量便利
    // 【便利重载动机】JsonNode("name") 若无此重载，const char* 会被
    //   std::string 版隐式转换接住（多一次临时 string 构造）—— 直接重载
    //   一步到位；显式重载也避免了"char* 落到 bool 上"的经典坑
    //   （裸指针可隐式转 bool！没有这个重载 JsonNode("x") 会变成 bool true）。
    explicit JsonNode(Array&& a)             : v(std::move(a)) {}
    explicit JsonNode(Object&& o)            : v(std::move(o)) {}
    // 【只留 && 版本的取舍】Array/Object 只提供移动构造：它们是解析器
    //   "现做现交"的临时对象，拷贝语义（深拷贝整棵子树）又贵又没必要。

    // ====== 类型查询 ======
    // ⭐ variant 的 index() 返回当前持有的索引
    // 【技术原理】index() 是 O(1) 读标签（不触碰值本身）；
    //   static_cast<Type>(v.index()) 依赖"枚举值 == variant 索引"的约定。
    Type type() const { return static_cast<Type>(v.index()); }
    // isXxx 一族：语义化封装（调用方写 isString() 比 type()==Type::String 可读）
    bool isNull() const { return type() == Type::Null; }
    bool isBool()   const { return type() == Type::Bool; }
    bool isNumber() const { return type() == Type::Number; }
    bool isString() const { return type() == Type::String; }
    bool isArray()  const { return type() == Type::Array; }
    bool isObject() const { return type() == Type::Object; }

    // ====== 访问器（先裸版，下一步再升级）======
    // 【契约】类型不对抛 TypeMismatchError（而非 std::get 的 bad_variant_access
    //   —— 后者消息不含上下文）；const& 版本返回只读引用（零拷贝）。
    bool                asBool()   const;
    double              asNumber() const;
    const std::string&  asString() const;
    const Array&        asArray()  const;
    const Object&       asObject() const;
    Array&  asArray();           // 可变版本
    Object& asObject();
    // 【const/非const 重载对】同名同参仅 const 不同的重载：const 对象只能
    //   调 const 版 —— 这是 STL 容器（如 vector::operator[]）的标准模式。

    // ====== 容器便捷操作 ======
    // 【接口设计】operator[] 模仿原生 JSON 访问手感：arr[0] / obj["name"]；
    //   越界/缺 key 抛异常（fail-fast），不做"静默插入"（那是 nlohmann 的
    //   便利取舍，教学版选择错误显式暴露）。
    JsonNode& operator[](size_t idx);                  // arr[0]
    JsonNode& operator[](const std::string& key);      // obj["name"]
    size_t size() const;
    bool   contains(const std::string& key) const;
    // 路径查询：obj.at("user.address.city")
    // 【设计动机】KV 数据库的多级路径直达入口：按 '.' 逐层下钻，
    //   任一层失败即抛 KeyNotFoundError —— 一个 at() 顶一串嵌套 operator[]。
    JsonNode& at(const std::string& path);
    const JsonNode& at(const std::string& path) const;

private:
    Value v;                // 持有 6 种之一。实际存储的值
    // 【内存视角】variant 的大小 ≈ 最大成员（Object/map）+ 对齐 + 标签，
    //   所有 JsonNode 等大（对 vector 缓存友好）；子节点经由 unique_ptr
    //   间接持有，析构沿指针递归进行（unique_ptr 自动 delete → JsonNode
    //   析构 → 其 variant 析构 → 子 unique_ptr 析构……整棵树自动释放）。
};


// ====== 工厂函数（类外）======
// 【设计动机】makeXxx() 让"造节点指针"一句话完成，且移动语义贯穿全程
//   （make_unique 直接在堆上原地构造，参数再 move 进去，零拷贝）；
//   对比裸写 std::make_unique<JsonNode>(JsonNode::Array{}) 之啰嗦。
inline JsonNodePtr makeNull()                  { return std::make_unique<JsonNode>(); }
inline JsonNodePtr makeBool(bool b)            { return std::make_unique<JsonNode>(b); }
inline JsonNodePtr makeNumber(double n)        { return std::make_unique<JsonNode>(n); }
inline JsonNodePtr makeString(std::string s)   { return std::make_unique<JsonNode>(std::move(s)); }
// 按值收 string 再 move 进去：调用方无论传左值右值都恰好一次拷贝/零拷贝
inline JsonNodePtr makeArray()                 { return std::make_unique<JsonNode>(JsonNode::Array{}); }
inline JsonNodePtr makeObject()                { return std::make_unique<JsonNode>(JsonNode::Object{}); }

}   // namespace jsonkv
