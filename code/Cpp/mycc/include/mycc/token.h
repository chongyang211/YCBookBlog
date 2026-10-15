#pragma once

#include <string>    // std::string（Token 载荷之一）
#include <variant>   // std::variant：类型安全的标签联合
#include <iostream>  // std::ostream（dump 用）

namespace Mycc {

// ============================================================================
// 【模块定位】token.h —— 词法单元：编译器第一段的产物（教程阶段②）
// ----------------------------------------------------------------------------
// 【设计思想】词法分析的本质是"给无结构的字符流打上结构标签"：
//   源码 "x = 1 + 2;" → [Ident("x"), Assign, Number(1), Plus, Number(2), ...]
//   TokKind 枚举回答"这是什么"，variant 载荷回答"具体值是多少"。
// ============================================================================

// Token 类别
// 【enum class 强类型枚举】作用域受限（必须写 TokKind::Plus），
//   不隐式转 int —— switch 分发安全且可读；分组注释即词法规范的"词类表"
enum class TokKind {
    // 字面量
    Number,         // 整数或浮点：42、3.14
    // 【统一 Number】语言里数字只有一种运行时类型（double），词法层不区分
    //   整数/浮点 —— 简化编译器（真实 C 编译器要分 int/float/double/后缀）
    String,         // 字符串："hello"
    True, False,    // 布尔：true、false
    // 【为什么 true/false 是独立词类】它们是"关键字"而非标识符 ——
    //   若按标识符处理，用户可以 let true = 1; 搞坏语言语义，词法层就锁死

    // 标识符与关键字
    Ident,          // 标识符：x、fib
    Let, Print, If, Else, While, Fn, Return,   // 关键字
    // 关键字 = 保留的标识符：词法层识别（查表），语法层直接按词类分派

    // 单/多字符运算符
    Plus, Minus, Star, Slash, Percent,     // + - * / %
    Assign,                                  // =
    // 【单字符 = 与双字符 == 分立】词法层"最长匹配"后各自成独立词类，
    //   语法层无需再拼字符判断（把复杂度前移到词法、语法更纯粹）
    EqEq, BangEq, Lt, Le, Gt, Ge,            // == != < <= > >=
    AndAnd, OrOr, Bang,                      // && || !

    LParen, RParen, LBrace, RBrace,          // ( ) { }
    Semicolon, Comma,                        // ; ,

    Eof,            // 输入结束哨兵
    // 【哨兵设计】EOF 是一个"真实 Token"而非"没有 Token" ——
    //   Parser 的 peek/expect 无需判空（永远有 token 可看），
    //   语法错误检查简化为"到结尾了还在等什么"的普通分支
};

// Token：tag + 载荷（数字 / 字符串 / 空）
struct Token {
    TokKind kind;
    std::variant<std::monostate, double, std::string> value;
    // 【三路载荷的设计】
    //   monostate —— "无载荷"：运算符/标点/关键字只有类别没有值；
    //      （variant 必须有默认备选，monostate 就是那个"空"）
    //   double    —— Number 的值（int/float 统一 double，见 TokKind 注释）；
    //   string    —— Ident 的名字 / String 的内容（一个备选两用）。
    //   【对比 C 风格】"int kind; char* str; double num;"三字段并存的写法
    //      要靠约定"哪种 kind 看哪个字段"——variant 把约定变成类型系统保证
    int line = 1;       // 出错时定位
    // 行号在词法阶段唯一可知（字符流没有 AST 的父子关系）——
    //   Token 是"行号信息"的运输载体，AST 节点构造时从这取

    Token() = default;
    // = default：vector<Token> 等容器需要默认构造；monostate 载荷天然安全
    Token(TokKind k, int ln = 1) : kind(k), line(ln) {}
    // 无载荷构造：运算符/关键字/EOF 用（monostate 默认就位）
    Token(TokKind k, double n, int ln = 1)            : kind(k), value(n), line(ln) {}
    Token(TokKind k, const std::string& s, int ln = 1): kind(k), value(s), line(ln) {}
    // 三个重载按载荷类型分工 —— 调用处 emplace_back(TokKind::Number, 3.14, line)
    //   直接匹配，不必手工构造 variant

    // 调试输出
    void dump(std::ostream& os) const;
};

// 把 TokKind 转成可读字符串（调试 + 错误信息用）
const char* tokKindName(TokKind k);
// 【返回 const char*】所有可能值都是字面量常量，返回指针零拷贝；
//   错误消息"expected ; but got Number"就靠它把枚举翻译给人看

}  // namespace Mycc
