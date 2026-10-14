// common/timer.hpp — 高精度 RAII 计时器
#pragma once
#include <chrono>

namespace mlab {

class Timer {
public:
    using Clock = std::chrono::steady_clock;
    Timer() : t0_(Clock::now()) {}

    double elapsed_ns() const {
        return (double)std::chrono::duration_cast<std::chrono::nanoseconds>(
            Clock::now() - t0_).count();
    }
    double elapsed_us() const { return elapsed_ns() / 1000.0; }
    double elapsed_ms() const { return elapsed_ns() / 1'000'000.0; }
    double elapsed_s()  const { return elapsed_ns() / 1'000'000'000.0; }

    void reset() { t0_ = Clock::now(); }

private:
    Clock::time_point t0_;
};

}  // namespace mlab
