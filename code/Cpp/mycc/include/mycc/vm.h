#pragma once

#include "chunk.h"
#include "value.h"
#include <unordered_map>
#include <vector>
#include <string>

namespace mycc {

// 调用栈帧：每个函数调用对应一个 CallFrame
struct CallFrame {
    const Chunk* chunk;       // 当前执行的字节码
    size_t       ip;          // 指令指针（chunk->code 的下标）
    size_t       slotBase;    // 该函数局部变量在求值栈中的起始位置
    std::string  name;        // 调试用
};

class VM {
public:
    // 注入：编译期产物（主 chunk + 函数表）
    void load(Chunk topChunk, std::unordered_map<std::string, Chunk> fns);

    // 执行：从主 chunk 开始
    void run();

private:
    // ----- 编译期产物 -----
    Chunk                                     top_;
    std::unordered_map<std::string, Chunk>    fns_;
    std::vector<const Chunk*>                 fnTable_;     // 索引化访问，加速 CALL
    std::unordered_map<std::string, size_t>   fnIndex_;     // 函数名 → fnTable_ 索引

    // ----- 运行期状态 -----
    std::vector<Value>     stack_;             // 求值栈
    std::vector<CallFrame> frames_;            // 调用栈
    std::unordered_map<std::string, Value> globals_;

    // ----- 栈操作辅助 -----
    void  push(Value v) { stack_.push_back(std::move(v)); }
    Value pop()          { Value v = std::move(stack_.back()); stack_.pop_back(); return v; }
    Value& peek(size_t depth = 0) { return stack_[stack_.size() - 1 - depth]; }

    // ----- 字节码解码 -----
    uint8_t  readByte();
    uint16_t readU16();
    Constant readConstant();

    // ----- 算术辅助 -----
    void binaryArith(OpCode op, int line);
    void binaryCmp  (OpCode op, int line);

    // ----- 错误 -----
    [[noreturn]] void runtimeError(const std::string& msg, int line);
};

}  // namespace mycc
