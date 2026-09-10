#include "token.h"

#include <stdlib.h>
#include <string.h>

const char *tok_kind_name(TokKind k) {
    switch (k) {
    case TK_NUMBER:  return "Number";
    case TK_STRING:  return "String";
    case TK_TRUE:    return "True";    case TK_FALSE: return "False";
    case TK_IDENT:   return "Ident";
    case TK_VAR:     return "Var";     case TK_PRINT: return "Print";
    case TK_IF:      return "If";      case TK_ELSE:  return "Else";
    case TK_WHILE:   return "While";   case TK_FN:    return "Fn";
    case TK_RETURN:  return "Return";
    case TK_PLUS:    return "+";       case TK_MINUS: return "-";
    case TK_STAR:    return "*";       case TK_SLASH: return "/";
    case TK_PERCENT: return "%";
    case TK_ASSIGN:  return "=";       case TK_EQ:    return "==";
    case TK_NE:      return "!=";      case TK_LT:    return "<";
    case TK_LE:      return "<=";      case TK_GT:    return ">";
    case TK_GE:      return ">=";
    case TK_ANDAND:  return "&&";      case TK_OROR:  return "||";
    case TK_BANG:    return "!";
    case TK_LPAREN:  return "(";       case TK_RPAREN:return ")";
    case TK_LBRACE:  return "{";       case TK_RBRACE:return "}";
    case TK_SEMI:    return ";";       case TK_COMMA: return ",";
    case TK_EOF:     return "<EOF>";
    }
    return "?";
}

void token_dump(const Token *t, FILE *out) {
    fprintf(out, "Token(%s", tok_kind_name(t->kind));
    /* ⭐ 必须先看 kind，才能访问 union 哪个分支 */
    switch (t->kind) {
    case TK_NUMBER:
        fprintf(out, ", %g", t->as.number);
        break;
    case TK_STRING: case TK_IDENT:
        fprintf(out, ", \"%s\"", t->as.string);
        break;
    default:
        break;
    }
    fprintf(out, ")");
}

void token_free(Token *t) {
    if (t->kind == TK_STRING || t->kind == TK_IDENT) {
        free(t->as.string);
        t->as.string = NULL;
    }
}
