#pragma once
#include <cstdint>
#include <variant>
#include <string>

namespace mm {

// 14 条指令,占用 op 字段 [0..13],上限 31 (5 bit)
enum class Op : uint8_t {
    NOP = 0,
    MOV,        // rd = src (视 mode 决定 src 从哪来)
    ADD, SUB, AND, OR, XOR,
    JMP,        // PC = target (无条件)
    JZ,         // if (R[rs1] == 0) PC = target
    CALL,       // 压栈返回地址,跳转到 target
    RET,        // 从栈弹出返回地址
    LOAD,       // rd = MEM[base + disp]
    STORE,      // MEM[base + disp] = R[rs1]
    HALT,       // 停机 (让 mmach 退出主循环)
};

// 5 种寻址方式,mode 字段 [0..4],上限 7 (3 bit)
enum class Mode : uint8_t {
    IMM = 0,   // 立即数:      operand = 18bit signed
    REG = 1,   // 寄存器:      operand[2:0] = rs2
    DIR = 2,   // 直接内存地址: operand = 18bit unsigned addr
    IND = 3,   // 间接:        operand[2:0] = rs2, MEM[R[rs2]]
    BAS = 4,   // 基址+偏移:    operand = 18bit signed disp
    // REL/IDX 留作扩展,专栏 07 §3.4 有对照
};

// ---- 五种指令"形状",std::variant 组合成 Inst ----
struct RForm { Op op; uint8_t rd, rs1, rs2; };            // ADD rd, rs1, rs2
struct IForm { Op op; uint8_t rd, rs1; int32_t imm; };     // ADD rd, rs1, #imm 或 MOV rd, #imm
struct MForm { Op op; uint8_t rd, base; int32_t disp; };   // LOAD/STORE rd, [base + disp]
struct JForm { Op op; uint8_t rs1; uint32_t target; };     // JMP/JZ/CALL target
struct SForm { Op op; };                                    // NOP/RET/HALT (无操作数)

using Inst = std::variant<RForm, IForm, MForm, JForm, SForm>;

// ---- 编解码 ----
uint32_t   encode(const Inst& inst);
Inst       decode(uint32_t raw);

// ---- 反汇编 (给 mdisas 用) ----
std::string disasm(const Inst& inst);
std::string opName(Op op);
std::string modeName(Mode m);

}  // namespace mm
