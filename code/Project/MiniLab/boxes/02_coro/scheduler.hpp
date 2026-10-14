// scheduler.hpp — 单核协程调度器
#pragma once
#include "coroutine.hpp"
#include <deque>
#include <functional>
#include <memory>

namespace mlab {

class Scheduler {
public:
    void co_spawn(std::function<void()> fn, std::size_t stack = 8 * 1024) {
        ready_.emplace_back(std::make_unique<Coroutine>(std::move(fn), stack));
    }

    void run_until_empty() {
        getcontext(&main_ctx_);
        while (!ready_.empty()) {
            auto co = std::move(ready_.front()); ready_.pop_front();
            current_ = co.get();
            co->resume(&main_ctx_);
            current_ = nullptr;
            if (co->state() != Coroutine::Dead) ready_.push_back(std::move(co));
        }
    }

    void yield_current() { if (current_) current_->yield(); }

    static Scheduler*& inst() { static Scheduler* s = nullptr; return s; }

private:
    ucontext_t main_ctx_{};
    std::deque<std::unique_ptr<Coroutine>> ready_;
    Coroutine* current_ = nullptr;
};

inline void co_yield_() { if (Scheduler::inst()) Scheduler::inst()->yield_current(); }

}  // namespace mlab
