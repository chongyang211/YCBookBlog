//
// Created by 杨充 on 2026/6/5.
//
// ============================================================================
// 【文件定位】JsonParser.h —— 递归下降解析器（JSON 文本 → JsonNode 树）
// ----------------------------------------------------------------------------
// 【算法思想·递归下降】JSON 的语法天然是"每层看一个首字符就知道该调谁"：
//   '{'→parseObject、'['→parseArray、'"'→parseString、'-'或数字→parseNumber、
//   'n/t/f'→parseLiteral —— parseValue 按首字符分派，子函数内部再递归调
//   parseValue —— 语法结构与函数调用结构同构，这是最贴近文法的解析器写法。
// 【游标模型】pos/line/col 三元组就是"游标"，peek/consume/match 是游标的
//   三个原子动作 —— 解析全部逻辑都建立在这三个动作之上。
// ============================================================================

#pragma once
#include "JsonNode.h"      // 解析产物：JsonNode / JsonNodePtr / 工厂函数
#include <string>          // std::string
#include <string_view>     // std::string_view：零拷贝字符串视图（C++17）

namespace jsonkv {

class JsonParser {
private:
    std::string_view src;   // 待解析的 JSON 源字符串（轻量引用，不拷贝）
    // 【技术原理·string_view】只存"指针+长度"（16 字节），不复制内容 ——
    //   解析器只读不改，视图是完美匹配；【危险边界】view 不延长所指数据的
    //   生命周期：调用方必须保证源字符串活得比 parser 久（KvDatabase::load
    //   里特意先存局部变量再构造 parser，就是在防悬垂 —— 详见其注释）。
    size_t pos = 0;         // 当前解析到的位置（索引）
    size_t line = 1, col = 1; // 当前行号和列号（用于报错时定位）
    // 【行号列号的代价】consume() 每读一个字符都维护 line/col —— 平时白费，
    //   报错那一刻价值千金（错误定位是解析器的核心体验）。

    // ===== 游标工具 =====
    // 【三动作模型】peek（看不吃）/ consume（吃掉并前进）/ match（条件吃）——
    //   组合这三个原子动作可以表达一切"逐字符"解析逻辑。
    char   peek() const;         // 看当前字符（不前进）
    char   consume();            // 吃掉当前字符并前进
    bool   match(char c);        // 当前字符如果是 c 就吃掉
    void   skipWhitespace();
    // [[noreturn]]：向编译器承诺"此函数必不返回"（只抛异常）——
    //   有了它 parseValue 末尾的 error(...) 调用不需要冗余 return
    [[noreturn]] void error(const std::string& msg);

    // 字面量解析
    JsonNodePtr parseValue();        // 总分派：按首字符路由到具体 parse*
    JsonNodePtr parseLiteral();      // null / true / false 三个关键字
public:
    explicit JsonParser(std::string_view s) : src(s) {}
    // explicit：单参构造禁隐式转换（避免任意字符串被静默变 parser）；
    // 按值收 string_view（16 字节，拷贝比引用间接访问还便宜）
    // 调试用：打印源串信息
    void debugDump() const;
    JsonNodePtr parse();         // 唯一入口：完整解析 + 尾部垃圾检查
    // 【接口说明】parseNumber/String/Array/Object 声明为 public 并非疏漏：
    //   教学版刻意暴露中间步骤便于分阶段测试（教程阶段③④逐步点亮它们），
    //   工程版应收回 private —— 这里保留教学可测性。
    JsonNodePtr parseNumber();
    JsonNodePtr parseString();
    JsonNodePtr parseArray();
    JsonNodePtr parseObject();
};

}
