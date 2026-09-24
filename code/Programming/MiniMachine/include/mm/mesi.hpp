#pragma once
#include "mm/storage.hpp"
#include <array>
#include <cstdint>
#include <vector>

namespace mm {

enum class MState : uint8_t { I = 0, S, E, M };
inline const char* mstateName(MState s) {
    static const char* n[] = {"I","S","E","M"};
    return n[static_cast<int>(s)];
}

// 总线广播的 3 种事件
enum class BusEvent : uint8_t { BusRd, BusRdX, BusUpgr };

class CoreCache;   // 前向声明

// 共享总线:所有核挂载,广播事件
class MesiBus {
public:
    void attach(CoreCache* core) { cores_.push_back(core); }

    // 由 CoreCache 调用:发起一次广播,让所有其他核嗅探
    // 返回值:是否有其他核持有该行 (供 BusRd 决定 E vs S)
    bool broadcast(BusEvent ev, uint32_t line_addr, CoreCache* from);

    // 统计
    uint64_t nBroadcasts() const { return nBroadcasts_; }
    uint64_t nInvalidations() const { return nInvalidations_; }

private:
    std::vector<CoreCache*> cores_;
    uint64_t nBroadcasts_    = 0;
    uint64_t nInvalidations_ = 0;
};

// 每个核私有的 L1(简化:直接映射)
class CoreCache : public Storage {
public:
    static constexpr uint32_t kLine = 64;
    static constexpr uint32_t kNumLines = 64;   // 64 行 = 4KB L1 (教学够用)

    CoreCache(int coreId, MesiBus& bus, Storage& mem);

    const char* name() const override;
    uint8_t read8 (uint32_t addr) override;
    void    write8(uint32_t addr, uint8_t v) override;

    // 被 MesiBus 调用:嗅探
    // 返回值:本核是否持有该行(用于 BusRd 决定共享)
    bool onSnoop(BusEvent ev, uint32_t line_addr);

    // 统计
    uint64_t transitions[4][4] = {};    // [from][to] 状态迁移计数

    int coreId() const { return coreId_; }
    void dump() const override;

private:
    struct Line {
        MState  state = MState::I;
        uint32_t tag = 0;
        std::array<uint8_t, kLine> data{};
    };
    uint32_t lineIdx(uint32_t addr) const { return (addr / kLine) % kNumLines; }
    uint32_t tagOf  (uint32_t addr) const { return addr / kLine / kNumLines; }
    uint32_t offset (uint32_t addr) const { return addr % kLine; }
    // 行首地址
    uint32_t lineBase(uint32_t addr) const { return addr & ~(kLine - 1); }
    void setState(Line& L, MState s);
    void fillFromMem(Line& L, uint32_t addr);

    int coreId_;
    MesiBus& bus_;
    Storage& mem_;
    std::array<Line, kNumLines> lines_;
    std::string name_;
};

}  // namespace mm
