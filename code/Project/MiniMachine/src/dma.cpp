#include "mm/dma.hpp"
#include "mm/log.hpp"

namespace mm {

bool DmaEngine::tick() {
    if (queue_.empty()) return false;
    Task& t = queue_.front();
    uint32_t n = std::min(kBytesPerTick, t.remaining);
    for (uint32_t i = 0; i < n; ++i) {
        uint8_t v = mem_.read8(t.src + i);
        mem_.write8(t.dst + i, v);
    }
    t.src += n; t.dst += n; t.remaining -= n;
    if (t.remaining == 0) {
        MM_INFO("dma", "task done, raising IRQ %u", t.irq);
        io_.raiseIrq(t.irq);
        queue_.pop_front();
        ++nDone_;
    }
    return true;
}

}  // namespace mm
