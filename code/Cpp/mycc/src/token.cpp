// ============================================================================
// 【文件定位】token.cpp —— Token 的调试输出与词类名翻译（小工具的实现）
// ============================================================================
#include "mycc/token.h"

namespace Mycc {

// 词类 → 可读字符串：给错误消息和 dump 用（switch 全覆盖 + 兜底 return）
const char* tokKindName(TokKind k) {
    switch (k) {
        case TokKind::Number: return "Number";
        case TokKind::String: return "String";
        case TokKind::True:   return "True";    case TokKind::False: return "False";
        case TokKind::Ident:  return "Ident";
        case TokKind::Let:    return "let";     case TokKind::Print: return "print";
        case TokKind::If:     return "if";      case TokKind::Else:  return "else";
        case TokKind::While:  return "while";   case TokKind::Fn:    return "fn";
        case TokKind::Return: return "return";
        // 关键字返回源码拼写（小写）——错误消息读起来像"你打错的源码"
        case TokKind::Plus:   return "+";       case TokKind::Minus: return "-";
        case TokKind::Star:   return "*";       case TokKind::Slash: return "/";
        case TokKind::Percent:return "%";
        case TokKind::Assign: return "=";       case TokKind::EqEq:  return "==";
        case TokKind::BangEq: return "!=";      case TokKind::Lt:    return "<";
        case TokKind::Le:     return "<=";      case TokKind::Gt:    return ">";
        case TokKind::Ge:     return ">=";
        case TokKind::AndAnd: return "&&";      case TokKind::OrOr:  return "||";
        case TokKind::Bang:   return "!";
        // 运算符返回符号本身 —— "expected ; but got ==" 直接可读
        case TokKind::LParen: return "(";       case TokKind::RParen:return ")";
        case TokKind::LBrace: return "{";       case TokKind::RBrace:return "}";
        case TokKind::Semicolon: return ";";    case TokKind::Comma: return ",";
        case TokKind::Eof:    return "<EOF>";
    }
    return "?";
    // switch 覆盖全部枚举值后的兜底（防未来加词类忘更新 —— 防御性收尾）
}

// 调试输出：Token(Number, 3.14) / Token(Ident, "x") / Token(Plus) 三种形态
void Token::dump(std::ostream& os) const {
    os << "Token(" << tokKindName(kind);
    std::visit([&os](auto&& v) {
        // visit 按 variant 当前载荷实例化对应分支
        using T = std::decay_t<decltype(v)>;
        // monostate 无分支命中 if constexpr 都不成立 → 只打印类别（运算符形态）
        if constexpr (std::is_same_v<T, double>)            os << ", " << v;
        else if constexpr (std::is_same_v<T, std::string>)  os << ", \"" << v << "\"";
        // if constexpr：不匹配的分支不实例化 —— monostate 进来两个分支都
        //   不生成代码，编译安全（普通 if 会因类型不符编译失败）
    }, value);
    os << ")";
}

}  // namespace Mycc
