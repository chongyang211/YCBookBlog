#pragma once

#include "opcode.h"
#include <cstdint>
#include <ostream>
#include <string>
#include <vector>
#include <variant>

namespace mycc {

// 常量池中的元素：数字、字符串、布尔都用它表示
using Constant = std::variant<double, std::string, bool>;

// ========================================
//  Chunk = 一段字节码（一个函数 / 一段顶层代码 = 一个 Chunk）
// ========================================
class Chunk {
public:
    std::vector<uint8_t>  code;        // 字节码流
    std::vector<Constant> constants;   // 常量池
    std::vector<int>      lines;       // 与 code 一一对应的行号
    std::string           name;        // 调试用：函数名 / "<top>"

    Chunk() = default;
    explicit Chunk(std::string n) : name(std::move(n)) {}

    // ----- 写入辅助 -----
    void emit(uint8_t byte, int line) {
        code.push_back(byte);
        lines.push_back(line);
    }
    void emit(OpCode op, int line) { emit(static_cast<uint8_t>(op), line); }

    // 写入 1 字节指令 + 2 字节小端操作数
    void emitWithU16(OpCode op, uint16_t operand, int line) {
        emit(op, line);
        emit(operand & 0xFF, line);
        emit((operand >> 8) & 0xFF, line);
    }

    // 添加常量；若已存在则复用（去重）
    uint16_t addConstant(const Constant& c);

    // ----- 跳转回填核心 API -----
    size_t emitJump(OpCode op, int line);   // 写入跳转指令 + 占位，返回占位下标
    void   patchJump(size_t idx);           // 回填相对偏移
    void   emitLoop(size_t target, int line);

    // ----- 反汇编 -----
    void disassemble(std::ostream& os) const;
};

}  // namespace mycc
