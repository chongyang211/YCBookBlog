#include "mm/mesi.hpp"
#include "mm/log.hpp"
#include <cstdio>

namespace mm {

CoreCache::CoreCache(int id, MesiBus& bus, Storage& mem)
    : coreId_(id), bus_(bus), mem_(mem)
{
    name_ = "C" + std::to_string(id) + "$";
    bus_.attach(this);
}

const char* CoreCache::name() const { return name_.c_str(); }

void CoreCache::setState(Line& L, MState s) {
    MState from = L.state;
    ++transitions[static_cast<int>(from)][static_cast<int>(s)];
    L.state = s;
}

void CoreCache::fillFromMem(Line& L, uint32_t addr) {
    uint32_t base = lineBase(addr);
    for (uint32_t i = 0; i < kLine; ++i)
        L.data[i] = mem_.read8(base + i);
    L.tag = tagOf(addr);
}

// 处理器读请求
uint8_t CoreCache::read8(uint32_t addr) {
    Line& L = lines_[lineIdx(addr)];
    if (L.state != MState::I && L.tag == tagOf(addr)) {
        // 命中
        ++hits_;
        MM_TRACE("mesi", "C%d READ  0x%X hit  state=%s",
            coreId_, addr, mstateName(L.state));
        return L.data[offset(addr)];
    }
    // 未命中:广播 BusRd
    ++misses_;
    bool othersHave = bus_.broadcast(BusEvent::BusRd, lineBase(addr), this);
    fillFromMem(L, addr);
    setState(L, othersHave ? MState::S : MState::E);
    MM_TRACE("mesi", "C%d READ  0x%X miss → %s",
        coreId_, addr, mstateName(L.state));
    return L.data[offset(addr)];
}

// 处理器写请求
void CoreCache::write8(uint32_t addr, uint8_t v) {
    Line& L = lines_[lineIdx(addr)];
    bool hit = (L.state != MState::I && L.tag == tagOf(addr));
    if (hit) {
        ++hits_;
        switch (L.state) {
            case MState::M:  break;
            case MState::E:  setState(L, MState::M); break;
            case MState::S:  bus_.broadcast(BusEvent::BusUpgr, lineBase(addr), this);
                             setState(L, MState::M); break;
            case MState::I:  break;   // 不会走到
        }
    } else {
        ++misses_;
        bus_.broadcast(BusEvent::BusRdX, lineBase(addr), this);
        fillFromMem(L, addr);
        setState(L, MState::M);
    }
    L.data[offset(addr)] = v;
    MM_TRACE("mesi", "C%d WRITE 0x%X=%02X state=%s",
        coreId_, addr, v, mstateName(L.state));
}

// 嗅探处理:被 Bus 广播叫醒
bool CoreCache::onSnoop(BusEvent ev, uint32_t line_addr) {
    uint32_t idx = lineIdx(line_addr);
    Line& L = lines_[idx];
    if (L.state == MState::I || L.tag != tagOf(line_addr)) return false;
    // 本核持有该行
    switch (ev) {
        case BusEvent::BusRd:
            // 别人想读:M/E → S(M 要写回内存,略);S 不变
            if (L.state == MState::M) {
                // 写回内存
                uint32_t base = line_addr;
                for (uint32_t i = 0; i < kLine; ++i) mem_.write8(base + i, L.data[i]);
            }
            setState(L, MState::S);
            break;
        case BusEvent::BusRdX:
        case BusEvent::BusUpgr:
            // 别人想独占:变 I
            if (L.state == MState::M) {
                uint32_t base = line_addr;
                for (uint32_t i = 0; i < kLine; ++i) mem_.write8(base + i, L.data[i]);
            }
            setState(L, MState::I);
            break;
    }
    return true;
}

bool MesiBus::broadcast(BusEvent ev, uint32_t line_addr, CoreCache* from) {
    ++nBroadcasts_;
    bool othersHave = false;
    for (auto* c : cores_) {
        if (c == from) continue;
        bool held = c->onSnoop(ev, line_addr);
        if (held) {
            othersHave = true;
            if (ev == BusEvent::BusRdX || ev == BusEvent::BusUpgr)
                ++nInvalidations_;
        }
    }
    return othersHave;
}

void CoreCache::dump() const {
    std::printf("│  %s : hits=%llu misses=%llu",
        name(), (unsigned long long)hits_, (unsigned long long)misses_);
    static const char* n[] = {"I","S","E","M"};
    std::printf("  trans:");
    for (int f = 0; f < 4; ++f)
        for (int t = 0; t < 4; ++t)
            if (transitions[f][t])
                std::printf(" %s→%s:%llu", n[f], n[t],
                    (unsigned long long)transitions[f][t]);
    std::puts("");
}

}  // namespace mm
