#pragma once
#include <array>
#include <cstdint>
#include <functional>
#include <string>

namespace mm {

// 异常类型
enum class ExType : uint8_t { Interrupt, Trap, Fault, Abort };

// 异常帧: handler 拿到的现场信息
struct ExFrame {
    uint32_t vec;         // 向量号 (0..255)
    uint32_t pc;          // 触发时的 PC
    uint32_t errCode;     // 附加错误码 (如 #PF 的地址)
    ExType   type;
    const char* mnemonic; // "#DE" / "#PF" / "syscall" ...
};

using ExHandler = std::function<void(ExFrame&)>;

// 256 项向量表
class Ivt {
public:
    static constexpr int kSize = 256;

    // 常见异常号 (Intel 约定)
    enum : uint32_t {
        VEC_DE = 0,   // Divide Error (#DE)
        VEC_UD = 6,   // Undefined Instruction (#UD)
        VEC_GP = 13,  // General Protection (#GP)
        VEC_PF = 14,  // Page Fault (#PF)
        VEC_SYSCALL = 128,  // 用户态 syscall (Linux 用 0x80)
    };

    Ivt();

    void setHandler(uint32_t vec, ExType t, const char* mnemonic, ExHandler h);
    void raise(uint32_t vec, ExFrame frame);

    // 统计
    uint64_t nRaised() const { return nRaised_; }
    uint64_t nHandled() const { return nHandled_; }

private:
    struct Entry { ExType type; const char* mnemonic; ExHandler handler; };
    std::array<Entry, kSize> tab_;
    uint64_t nRaised_ = 0, nHandled_ = 0;
};

}  // namespace mm
