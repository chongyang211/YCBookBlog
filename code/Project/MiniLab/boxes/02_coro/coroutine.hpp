// coroutine.hpp — ucontext 有栈协程
#pragma once
#include <ucontext.h>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>

namespace mlab {

class Coroutine {
public:
    enum State { Ready, Running, Suspended, Dead };

    Coroutine(std::function<void()> fn, std::size_t stack_sz = 8 * 1024)
      : fn_(std::move(fn)), stack_(stack_sz) {
        getcontext(&ctx_);
        ctx_.uc_stack.ss_sp   = stack_.data();
        ctx_.uc_stack.ss_size = stack_.size();
        ctx_.uc_link          = nullptr;
        auto p = reinterpret_cast<uintptr_t>(this);
        int hi = static_cast<int>(p >> 32);
        int lo = static_cast<int>(p & 0xFFFFFFFF);
        makecontext(&ctx_, (void(*)())&Coroutine::entry, 2, hi, lo);
    }

    void resume(ucontext_t* caller) {
        caller_ = caller;
        state_  = Running;
        swapcontext(caller, &ctx_);
    }

    void yield() {
        state_ = Suspended;
        swapcontext(&ctx_, caller_);
    }

    State state() const { return state_; }

private:
    static void entry(int hi, int lo) {
        auto self = reinterpret_cast<Coroutine*>(
            (static_cast<uintptr_t>(static_cast<uint32_t>(hi)) << 32) |
             static_cast<uint32_t>(lo));
        self->fn_();
        self->state_ = Dead;
        swapcontext(&self->ctx_, self->caller_);
    }

    std::function<void()> fn_;
    std::vector<char>     stack_;
    ucontext_t            ctx_{};
    ucontext_t*           caller_ = nullptr;
    State                 state_  = Ready;
};

}  // namespace mlab
