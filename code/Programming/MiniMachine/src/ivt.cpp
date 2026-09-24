#include "mm/ivt.hpp"
#include "mm/log.hpp"

namespace mm {

Ivt::Ivt() {
    // 默认全部指向"未处理异常"提示
    for (auto& e : tab_) {
        e.type = ExType::Abort;
        e.mnemonic = "#??";
        e.handler = [](ExFrame& f){
            MM_ERR("ivt", "unhandled exception vec=%u pc=0x%X", f.vec, f.pc);
        };
    }
}

void Ivt::setHandler(uint32_t vec, ExType t, const char* mn, ExHandler h) {
    tab_[vec] = {t, mn, std::move(h)};
}

void Ivt::raise(uint32_t vec, ExFrame f) {
    ++nRaised_;
    auto& e = tab_[vec];
    f.type = e.type;
    f.mnemonic = e.mnemonic;
    MM_TRACE("ivt", "raise %s (vec=%u) at PC=0x%X", e.mnemonic, vec, f.pc);
    e.handler(f);
    ++nHandled_;
}

}  // namespace mm
