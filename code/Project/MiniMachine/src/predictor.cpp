#include "mm/predictor.hpp"

namespace mm {

Predictor::Predictor() { tab_.fill(1); }   // 弱不跳初始, 保守起步

bool Predictor::predict(uint32_t pc) const {
    return tab_[idx(pc)] >= 2;    // 2/3 = 预测跳, 0/1 = 预测不跳
}

void Predictor::update(uint32_t pc, bool taken) {
    uint8_t& c = tab_[idx(pc)];
    bool predicted = (c >= 2);
    if (predicted == taken) ++hits_;
    else                    ++misses_;

    if (taken) { if (c < 3) ++c; }
    else       { if (c > 0) --c; }
}

}  // namespace mm
