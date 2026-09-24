#pragma once

namespace mm {

// 每个"五大部件"都实现这个基类 (阶段 ①)
// 独立成头文件是为了断开 machine.hpp ↔ storage.hpp ↔ cpu.hpp 的循环包含
class Component {
public:
    virtual ~Component() = default;
    virtual const char* name() const = 0;
    virtual void dump() const = 0;   // 打印自身状态,给 REPL 的 dump 命令用
};

}  // namespace mm
