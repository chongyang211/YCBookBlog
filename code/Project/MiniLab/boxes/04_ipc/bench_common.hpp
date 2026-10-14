// bench_common.hpp — IPC benchmark 共享骨架
#pragma once
#include <sys/wait.h>
#include <unistd.h>
#include <chrono>
#include <cstdio>
#include "fmt_shim.hpp"

namespace mlab::ipc {

using Clock = std::chrono::steady_clock;

template <class ParentFn, class ChildFn>
void fork_run(ParentFn parent, ChildFn child) {
    pid_t pid = ::fork();
    if (pid < 0) { perror("fork"); std::_Exit(1); }
    if (pid == 0) { child(); std::_Exit(0); }
    else          { parent(); int st; ::waitpid(pid, &st, 0); }
}

inline void sync_stdio() { setvbuf(stdout, nullptr, _IONBF, 0); }

}  // namespace mlab::ipc
