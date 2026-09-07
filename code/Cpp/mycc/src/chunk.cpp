#include "mycc/chunk.h"
#include <iomanip>
#include <ostream>
#include <stdexcept>

namespace mycc {

uint16_t Chunk::addConstant(const Constant& c) {
    // 去重：避免常量池膨胀
    for (size_t i = 0; i < constants.size(); ++i) {
        if (constants[i] == c) return static_cast<uint16_t>(i);
    }
    if (constants.size() >= 65535) {
        throw std::runtime_error("too many constants in one chunk");
    }
    constants.push_back(c);
    return static_cast<uint16_t>(constants.size() - 1);
}

size_t Chunk::emitJump(OpCode op, int line) {
    emit(op, line);
    emit(0xFF, line);                  // 占位低字节
    emit(0xFF, line);                  // 占位高字节
    return code.size() - 2;            // 返回占位低字节下标
}

void Chunk::patchJump(size_t idx) {
    // 从 idx+2（跳转指令的下一条）到当前末尾的距离
    size_t jump = code.size() - idx - 2;
    if (jump > 0xFFFF) {
        throw std::runtime_error("jump distance too large (>64KB)");
    }
    code[idx]     = jump & 0xFF;
    code[idx + 1] = (jump >> 8) & 0xFF;
}

void Chunk::emitLoop(size_t target, int line) {
    emit(OpCode::LOOP, line);
    // 当前末尾再 +2 是 LOOP 操作数自身的 2 字节
    size_t offset = code.size() + 2 - target;
    if (offset > 0xFFFF) {
        throw std::runtime_error("loop distance too large");
    }
    emit(offset & 0xFF, line);
    emit((offset >> 8) & 0xFF, line);
}

// ----- 反汇编 -----
static size_t simple(std::ostream& os, const char* name, size_t off) {
    os << name << "\n";
    return off + 1;
}

static size_t u16Operand(std::ostream& os, const char* name,
                         const Chunk& c, size_t off) {
    uint16_t arg = c.code[off + 1] | (c.code[off + 2] << 8);
    os << std::left << std::setw(16) << name << arg;
    if (std::string(name) == "CONST" ||
        std::string(name) == "LOAD_GLOBAL" ||
        std::string(name) == "STORE_GLOBAL") {
        os << "  ; ";
        std::visit([&](auto&& v) { os << v; }, c.constants[arg]);
    }
    os << "\n";
    return off + 3;
}

void Chunk::disassemble(std::ostream& os) const {
    os << "== " << name << " ==\n";
    size_t off = 0;
    while (off < code.size()) {
        os << std::right << std::setw(4) << std::setfill('0') << off << std::setfill(' ') << "  ";
        OpCode op = static_cast<OpCode>(code[off]);
        const char* name = opcodeName(op);
        switch (op) {
            case OpCode::CONST:  case OpCode::LOAD_GLOBAL: case OpCode::STORE_GLOBAL:
            case OpCode::LOAD_LOCAL: case OpCode::STORE_LOCAL:
            case OpCode::JUMP: case OpCode::JUMP_IF_FALSE:
            case OpCode::JUMP_IF_TRUE: case OpCode::POP_JUMP_IF_FALSE:
            case OpCode::LOOP: case OpCode::CALL:
                off = u16Operand(os, name, *this, off);
                break;
            default:
                off = simple(os, name, off);
                break;
        }
    }
}

}  // namespace mycc
