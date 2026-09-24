#pragma once
#include <cstdint>
#include <functional>
#include <unordered_map>
#include <vector>

namespace mm {

// IO 请求
enum class IoOp { Read, Write };

// 简化的 IO 设备:预置一段数据,分块 ready
class IoDevice {
public:
    IoDevice(std::vector<uint8_t> initial) : data_(std::move(initial)) {}
    size_t   size() const { return data_.size(); }
    uint8_t  get(size_t i) const { return data_[i]; }
    void     set(size_t i, uint8_t v) { data_[i] = v; }

private:
    std::vector<uint8_t> data_;
};

using IrqHandler = std::function<void(uint32_t /*irq_num*/)>;

// IO 控制器 (与 CPU / DMA 交互的中枢)
class IoController {
public:
    void registerIrq(uint32_t irq, IrqHandler h) { handlers_[irq] = std::move(h); }
    // 由设备或 DMA 引擎调用:触发中断
    void raiseIrq(uint32_t irq) {
        ++nIrqs_;
        auto it = handlers_.find(irq);
        if (it != handlers_.end()) it->second(irq);
    }

    uint64_t nIrqs() const { return nIrqs_; }

private:
    std::unordered_map<uint32_t, IrqHandler> handlers_;
    uint64_t nIrqs_ = 0;
};

}  // namespace mm
