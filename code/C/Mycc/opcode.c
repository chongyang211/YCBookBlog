#include "opcode.h"

const char *opcode_name(OpCode op) {
    switch (op) {
    case OP_CONST:  return "CONST";
    case OP_NIL:    return "NIL";
    case OP_TRUE:   return "TRUE";
    case OP_FALSE:  return "FALSE";
    case OP_ADD:    return "ADD";
    case OP_SUB:    return "SUB";
    case OP_MUL:    return "MUL";
    case OP_DIV:    return "DIV";
    case OP_MOD:    return "MOD";
    case OP_NEG:    return "NEG";
    case OP_EQ:     return "EQ";
    case OP_NEQ:    return "NEQ";
    case OP_LT:     return "LT";
    case OP_LE:     return "LE";
    case OP_GT:     return "GT";
    case OP_GE:     return "GE";
    case OP_NOT:    return "NOT";
    case OP_AND:    return "AND";
    case OP_OR:     return "OR";
    case OP_LOAD_GLOBAL:        return "LOAD_GLOBAL";
    case OP_STORE_GLOBAL:       return "STORE_GLOBAL";
    case OP_LOAD_LOCAL:         return "LOAD_LOCAL";
    case OP_STORE_LOCAL:        return "STORE_LOCAL";
    case OP_JUMP:               return "JUMP";
    case OP_JUMP_IF_FALSE:      return "JUMP_IF_FALSE";
    case OP_JUMP_IF_TRUE:       return "JUMP_IF_TRUE";
    case OP_POP_JUMP_IF_FALSE:  return "POP_JUMP_IF_FALSE";
    case OP_LOOP:               return "LOOP";
    case OP_CALL:               return "CALL";
    case OP_RETURN:             return "RETURN";
    case OP_PRINT:              return "PRINT";
    case OP_POP:                return "POP";
    case OP_DUP:                return "DUP";
    case OP_HALT:               return "HALT";
    }
    return "??";
}
