#pragma once

#include "chunk.h"    // Chunk / Constant
#include "value.h"    // Value / Nil / isTruthy
#include <unordered_map>
#include <vector>
#include <string>

namespace Mycc {

// ============================================================================
// 【模块定位】vm.h —— 栈式虚拟机（教程阶段⑦：字节码 → 执行）
// ----------------------------------------------------------------------------
// 【设计思想】fetch-decode-execute 主循环 + 帧栈：
//   ① 与 AST 树遍历求值的区别：无递归（C++ 栈不参与用户递归）——
//     用户的 fib(30) 深递归走的是堆上的 frames_/stack_，不受 C++ 栈深限制；
//   ② 调用即"切 chunk + 记 slotBase"，返回即"弹帧 + 栈截断" ——
//     实参就地变局部（零拷贝传参），返回值经栈顶传递；
//   ③ 行号表支持运行时错误的"line N"定位 + 调用栈回溯。
// ============================================================================

// 调用栈帧：每个函数调用对应一个 CallFrame
struct CallFrame {
    const Chunk* chunk;       // 当前执行的字节码
    // 【const 指针】字节码编译后只读 —— VM 只消费不修改（多个帧可指向
    //   同一函数 chunk，共享安全）
    size_t       ip;          // 指令指针（chunk->code 的下标）
    // "程序计数器"的 vector 版 —— 一切控制流指令都在改它
    size_t       slotBase;    // 该函数局部变量在求值栈中的起始位置
    // 【帧的分界线】locals 的绝对位置 = slotBase + 槽号；
    //   RETURN 时以 slotBase 为刀把整帧（实参+局部）从栈上截掉
    std::string  name;        // 调试用
    // 错误回溯时的 "in fib" 名字
};

class VM {
public:
    // 注入：编译期产物（主 chunk + 函数表）
    void load(Chunk topChunk, std::unordered_map<std::string, Chunk> fns);
    // 【load/run 分离】先装载后执行 —— REPL 的 dump 命令只编译不跑，
    //   未来扩展（多次 run 同一产物）也有位置放

    // 执行：从主 chunk 开始
    void run();

private:
    // ----- 编译期产物 -----
    Chunk                                     top_;   // 主字节码
    std::unordered_map<std::string, Chunk>    fns_;   // 函数名 → chunk
    std::vector<const Chunk*>                 fnTable_;     // 索引化访问，加速 CALL
    std::unordered_map<std::string, size_t>   fnIndex_;     // 函数名 → fnTable_ 索引
    // 【双表的性能设计】map<name,Chunk> 只适合"按名查一次"；运行期函数值
    //   以索引(size_t)在栈上流转 → CALL 需"索引→chunk"的 O(1) 数组 ——
    //   fnTable_ 就是那个数组（load 时从 fns_ 展开而来，见 vm.cpp）

    // ----- 运行期状态 -----
    std::vector<Value>     stack_;             // 求值栈
    // 【单栈设计】表达式求值与函数局部共用一条栈 —— 帧间用 slotBase 分界
    //   （Lua 同款；对比"每帧独立栈"的实现，单栈代码更少、拷贝更少）
    std::vector<CallFrame> frames_;            // 调用栈
    std::unordered_map<std::string, Value> globals_;
    // 全局变量表：名字（经常量池）→ 值 —— 生命周期 = 整个 run

    // ----- 栈操作辅助 -----
    void  push(Value v) { stack_.push_back(std::move(v)); }
    Value pop()          { Value v = std::move(stack_.back()); stack_.pop_back(); return v; }
    // pop 是"移动出"：值离开栈所有权归调用方（避免大 string 拷贝）
    Value& peek(size_t depth = 0) { return stack_[stack_.size() - 1 - depth]; }
    // depth=0 栈顶、1 栈顶下一个 —— CALL/STORE 的偷看窗口

    // ----- 字节码解码 -----
    uint8_t  readByte();
    uint16_t readU16();
    Constant readConstant();
    // 三个"读并前进"原语：全部从"当前帧的 chunk"按 ip 读取 ——
    //   与 Codegen 的 emit/emitWithU16/addConstant 严格互逆

    // ----- 算术辅助 -----
    void binaryArith(OpCode op, int line);
    void binaryCmp  (OpCode op, int line);
    // 主循环的"二元运算"组抽出成函数 —— switch case 共享类型检查逻辑

    // ----- 错误 -----
    [[noreturn]] void runtimeError(const std::string& msg, int line);
    // [[noreturn]]：只抛（RuntimeError + 调用栈回溯）—— 调用处免写 return
};

}  // namespace Mycc
