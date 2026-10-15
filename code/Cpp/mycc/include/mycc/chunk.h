#pragma once

#include "opcode.h"   // OpCode
#include <cstdint>
#include <ostream>
#include <string>
#include <vector>
#include <variant>

namespace Mycc {

// 常量池中的元素：数字、字符串、布尔都用它表示
using Constant = std::variant<double, std::string, bool>;
// 【三级载荷】CONST 指令的 payload —— 字面量的"仓库格式"。
//   运行期 Value 的 variant 多两型（Nil/函数索引），但那两类不走常量池：
//   nil 有专用指令、函数值由 LOAD_GLOBAL 从函数表取 —— 两个 variant 各司其职

// ========================================
//  Chunk = 一段字节码（一个函数 / 一段顶层代码 = 一个 Chunk）
// ========================================
// 【设计思想】"每函数一个 Chunk"是 Lua/Python 同款结构：主程序是 <top> chunk，
//   每个函数声明编译出独立 chunk —— 各有常量池/行号表/局部变量空间，
//   VM 的 CallFrame 以 chunk 为执行单元切换。三个数组一一对应下标。
class Chunk {
public:
    std::vector<uint8_t>  code;        // 字节码流
    // 指令流：uint8_t 一字节一单元（OpCode 的底层类型设计回报）；
    // "指令 + 2 字节操作数"都按字节依次铺开 —— 最贴近真实机器码的形态
    std::vector<Constant> constants;   // 常量池
    // CONST 指令的操作数是这里的下标（去重见 addConstant）
    std::vector<int>      lines;       // 与 code 一一对应的行号
    // 【行号表】每条指令（每个字节）记源码行 —— 运行时错误"line 7"的
    //   定位就靠它按 ip 查（vm.cpp 的 fr.chunk->lines[fr.ip]）
    std::string           name;        // 调试用：函数名 / "<top>"
    // 错误追踪里打出的名字（RuntimeError 的 "in fib" 栈回溯）

    Chunk() = default;
    explicit Chunk(std::string n) : name(std::move(n)) {}

    // ----- 写入辅助 -----
    void emit(uint8_t byte, int line) {
        // 字节级追加：code 与 lines 同步长（"每个字节都有行号"）
        code.push_back(byte);
        lines.push_back(line);
    }
    void emit(OpCode op, int line) { emit(static_cast<uint8_t>(op), line); }
    // OpCode 重载：调用处写 emit(OpCode::ADD, line) 不必手动转型

    // 写入 1 字节指令 + 2 字节小端操作数
    void emitWithU16(OpCode op, uint16_t operand, int line) {
        emit(op, line);
        emit(operand & 0xFF, line);         // 低字节在前
        emit((operand >> 8) & 0xFF, line);  // 高字节在后
        // 【小端编码】取址 & 0xFF / 移位 >> 8 —— 与 readU16 的
        //   lo | (hi << 8) 严格互逆；本机字节序无关（自定义编码格式）
    }

    // 添加常量；若已存在则复用（去重）
    uint16_t addConstant(const Constant& c);
    // 返回下标供 CONST/LOAD_GLOBAL 携带（实现在 chunk.cpp）

    // ----- 跳转回填核心 API -----
    // 【回填(backpatching)·教程§8.4 的核心】编译 if/while 时"跳多远"
    //   取决于"还没生成的代码长度" —— 根本无法预知！解法分两步：
    //   ① emitJump 先写跳转指令 + 0xFFFF 占位，记下占位下标；
    //   ② 目标代码全部生成完毕后，patchJump 用"当前位置"算出真实偏移，
    //      回头改写那 2 个占位字节。
    //   "先记账后结账" —— 汇编器两遍扫描、链接器重定位、JIT patch 同源。
    size_t emitJump(OpCode op, int line);   // 写入跳转指令 + 占位，返回占位下标
    void   patchJump(size_t idx);           // 回填相对偏移
    void   emitLoop(size_t target, int line);
    // 回跳专用：while 的 LOOP 指向"之前"的位置（目标是已知量 ——
    //   无需回填，当场算出相对偏移即可，见 chunk.cpp）

    // ----- 反汇编 -----
    void disassemble(std::ostream& os) const;
    // REPL 的 dump 命令入口：字节码 → 可读文本（教学调试链路终点）
};

}  // namespace Mycc
