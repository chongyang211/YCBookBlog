#include "mycc/opcode.h"

namespace Mycc {

const char* opcodeName(OpCode op) {
    switch (op) {
        case OpCode::CONST:        return "CONST";
        case OpCode::NIL:          return "NIL";
        case OpCode::TRUE:         return "TRUE";
        case OpCode::FALSE:        return "FALSE";
        case OpCode::ADD:          return "ADD";
        case OpCode::SUB:          return "SUB";
        case OpCode::MUL:          return "MUL";
        case OpCode::DIV:          return "DIV";
        case OpCode::MOD:          return "MOD";
        case OpCode::NEG:          return "NEG";
        case OpCode::EQ:           return "EQ";
        case OpCode::NEQ:          return "NEQ";
        case OpCode::LT:           return "LT";
        case OpCode::LE:           return "LE";
        case OpCode::GT:           return "GT";
        case OpCode::GE:           return "GE";
        case OpCode::NOT:          return "NOT";
        case OpCode::AND:          return "AND";
        case OpCode::OR:           return "OR";
        case OpCode::LOAD_GLOBAL:  return "LOAD_GLOBAL";
        case OpCode::STORE_GLOBAL: return "STORE_GLOBAL";
        case OpCode::LOAD_LOCAL:   return "LOAD_LOCAL";
        case OpCode::STORE_LOCAL:  return "STORE_LOCAL";
        case OpCode::JUMP:         return "JUMP";
        case OpCode::JUMP_IF_FALSE:return "JUMP_IF_FALSE";
        case OpCode::JUMP_IF_TRUE: return "JUMP_IF_TRUE";
        case OpCode::POP_JUMP_IF_FALSE: return "POP_JUMP_IF_FALSE";
        case OpCode::LOOP:         return "LOOP";
        case OpCode::CALL:         return "CALL";
        case OpCode::RETURN:       return "RETURN";
        case OpCode::PRINT:        return "PRINT";
        case OpCode::POP:          return "POP";
        case OpCode::DUP:          return "DUP";
        case OpCode::HALT:         return "HALT";
    }
    return "??";
}

}  // namespace Mycc
