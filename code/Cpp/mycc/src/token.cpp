#include "mycc/token.h"

namespace mycc {

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
        case TokKind::Plus:   return "+";       case TokKind::Minus: return "-";
        case TokKind::Star:   return "*";       case TokKind::Slash: return "/";
        case TokKind::Percent:return "%";
        case TokKind::Assign: return "=";       case TokKind::EqEq:  return "==";
        case TokKind::BangEq: return "!=";      case TokKind::Lt:    return "<";
        case TokKind::Le:     return "<=";      case TokKind::Gt:    return ">";
        case TokKind::Ge:     return ">=";
        case TokKind::AndAnd: return "&&";      case TokKind::OrOr:  return "||";
        case TokKind::Bang:   return "!";
        case TokKind::LParen: return "(";       case TokKind::RParen:return ")";
        case TokKind::LBrace: return "{";       case TokKind::RBrace:return "}";
        case TokKind::Semicolon: return ";";    case TokKind::Comma: return ",";
        case TokKind::Eof:    return "<EOF>";
    }
    return "?";
}

void Token::dump(std::ostream& os) const {
    os << "Token(" << tokKindName(kind);
    std::visit([&os](auto&& v) {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, double>)            os << ", " << v;
        else if constexpr (std::is_same_v<T, std::string>)  os << ", \"" << v << "\"";
    }, value);
    os << ")";
}

}  // namespace mycc
