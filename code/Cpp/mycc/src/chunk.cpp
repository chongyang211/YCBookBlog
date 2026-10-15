// ============================================================================
// 【文件定位】chunk.cpp —— 字节码容器实现：常量池/回填三剑客/反汇编
// ============================================================================
#include "mycc/chunk.h"
#include <iomanip>
#include <ostream>
#include <stdexcept>

namespace Mycc {

// 常量入池：先线性查重（命中复用），池满（>65535）拒绝
uint16_t Chunk::addConstant(const Constant& c) {
    // 去重：避免常量池膨胀
    for (size_t i = 0; i < constants.size(); ++i) {
        if (constants[i] == c) return static_cast<uint16_t>(i);
        // 【查重的双重收益】① 池子小（"name" 出现十次只存一份）；
        //   ② 指令流里同样常量共享下标 —— variant 的 == 对 double/string/bool
        //   全部按值比较（variant 的比较能力白拿）
    }
    if (constants.size() >= 65535) {
        throw std::runtime_error("too many constants in one chunk");
        // 【硬上限的来源】CONST 指令的操作数只有 2 字节 → 下标最大 65535 ——
        //   指令编码格式决定的物理边界，超了必须显式报错（静默截断=错乱）
    }
    constants.push_back(c);
    return static_cast<uint16_t>(constants.size() - 1);
}

// 发出跳转：指令 + 2 字节占位（0xFFFF），返回占位首字节下标
size_t Chunk::emitJump(OpCode op, int line) {
    emit(op, line);
    emit(0xFF, line);                  // 占位低字节
    emit(0xFF, line);                  // 占位高字节
    return code.size() - 2;            // 返回占位低字节下标
    // 【为什么占位是 0xFFFF】"明显非法"的最大偏移 —— 若忘记 patch，
    //   运行时会跳到荒谬位置，比"碰巧跳对"更容易暴露 bug（防御性默认值）
}

// 回填：用"从跳转指令末尾到当前位置"的真实距离改写占位
void Chunk::patchJump(size_t idx) {
    // 从 idx+2（跳转指令的下一条）到当前末尾的距离
    size_t jump = code.size() - idx - 2;
    // 【相对偏移的基准】VM 执行 JUMP 时 ip 已读过 2 字节操作数、
    //   指向"跳转指令的下一条" —— 所以偏移 = 目标 - (idx+2)，
    //   目标恰是"当前 code.size()"（回填时后续代码尚未生成）
    if (jump > 0xFFFF) {
        throw std::runtime_error("jump distance too large (>64KB)");
        // 2 字节偏移的物理上限 —— 单块超 64KB 的教学场景基本不触发
    }
    code[idx]     = jump & 0xFF;        // 回写低字节
    code[idx + 1] = (jump >> 8) & 0xFF; // 回写高字节
    // 【就地改写】bytecode 是 vector —— "等代码生成完再回头补数字"
    //   在内存容器里轻而易举（真实的汇编器对文件做第二遍扫描，同思想）
}

// 发出回跳：LOOP 指向之前的位置（目标已知，无需回填）
void Chunk::emitLoop(size_t target, int line) {
    emit(OpCode::LOOP, line);
    // 当前末尾再 +2 是 LOOP 操作数自身的 2 字节
    size_t offset = code.size() + 2 - target;
    // 【偏移计算的时机】先把 2 字节操作数的位置预留（code.size()+2 是
    //   "操作数写完后"的位置），再当场写死 —— 与 patchJump 的"事后回填"
    //   形成对照：前跳目标未知要回填，回跳目标已知可即写
    if (offset > 0xFFFF) {
        throw std::runtime_error("loop distance too large");
    }
    emit(offset & 0xFF, line);
    emit((offset >> 8) & 0xFF, line);
}

// ----- 反汇编 -----
// 【三个解码器】反汇编必须与编码互逆 —— 每条指令要么 1 字节、要么 3 字节，
//   disassemble 靠 switch 分类解码，off 推进即"指令长度即步长"。

static size_t simple(std::ostream& os, const char* name, size_t off) {
    // 无操作数指令：一行名字，推进 1 字节
    os << name << "\n";
    return off + 1;
}

static size_t u16Operand(std::ostream& os, const char* name,
                         const Chunk& c, size_t off) {
    uint16_t arg = c.code[off + 1] | (c.code[off + 2] << 8);
    // 按小端拼出操作数（与 emitWithU16 互逆）
    os << std::left << std::setw(16) << name << arg;
    // setw(16) 列对齐：指令名列宽固定，操作数对齐成列（肉眼可扫描）
    if (std::string(name) == "CONST" ||
        std::string(name) == "LOAD_GLOBAL" ||
        std::string(name) == "STORE_GLOBAL") {
        // 这三条的操作数是"常量池下标" —— 追加注释显示常量内容：
        //   "CONST 0        ; 3.14" 一眼看清 push 了什么
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
        // 偏移前缀 4 位补零（0000、0003……）—— 与 CMake 等工具的反汇编风格一致
        OpCode op = static_cast<OpCode>(code[off]);
        const char* name = opcodeName(op);
        switch (op) {
            case OpCode::CONST:  case OpCode::LOAD_GLOBAL: case OpCode::STORE_GLOBAL:
            case OpCode::LOAD_LOCAL: case OpCode::STORE_LOCAL:
            case OpCode::JUMP: case OpCode::JUMP_IF_FALSE:
            case OpCode::JUMP_IF_TRUE: case OpCode::POP_JUMP_IF_FALSE:
            case OpCode::LOOP: case OpCode::CALL:
                off = u16Operand(os, name, *this, off);
                // 带 2 字节操作数的 11 条指令 —— 3 字节步长
                break;
            default:
                off = simple(os, name, off);
                break;
        }
    }
}

}  // namespace Mycc
