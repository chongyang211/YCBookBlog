// philosopher.hpp — 5 哲学家状态 + 四种实现
#pragma once
#include <array>
#include <atomic>
#include <chrono>
#include <mutex>
#include <thread>

namespace mlab::lock {

constexpr int N = 5;

struct State {
    std::mutex forks[N];
    std::atomic<int> hold[N]    = {-1,-1,-1,-1,-1};
    std::atomic<int> hold2[N]   = {-1,-1,-1,-1,-1};
    std::atomic<int> waiting[N] = {-1,-1,-1,-1,-1};
    std::atomic<long> meals[N]  = {0,0,0,0,0};
};
inline State g_state;

inline void observed_lock(int phil, int fork_id, int slot) {
    g_state.waiting[phil].store(fork_id);
    g_state.forks[fork_id].lock();
    g_state.waiting[phil].store(-1);
    (slot == 0 ? g_state.hold[phil] : g_state.hold2[phil]).store(fork_id);
}
inline void observed_unlock(int phil, int fork_id, int slot) {
    (slot == 0 ? g_state.hold[phil] : g_state.hold2[phil]).store(-1);
    g_state.forks[fork_id].unlock();
}

// BUG 版：全部先左后右
inline void philosopher_bug(int i, std::atomic<bool>& stop) {
    int l = i, r = (i + 1) % N;
    while (!stop.load()) {
        observed_lock(i, l, 0);
        std::this_thread::sleep_for(std::chrono::microseconds(100));
        observed_lock(i, r, 1);
        g_state.meals[i]++;
        observed_unlock(i, r, 1);
        observed_unlock(i, l, 0);
    }
}

// FIX 1：锁排序
inline void philosopher_order(int i, std::atomic<bool>& stop) {
    int a = std::min(i, (i + 1) % N);
    int b = std::max(i, (i + 1) % N);
    while (!stop.load()) {
        observed_lock(i, a, 0);
        observed_lock(i, b, 1);
        g_state.meals[i]++;
        observed_unlock(i, b, 1);
        observed_unlock(i, a, 0);
    }
}

// FIX 2：奇偶反向
inline void philosopher_parity(int i, std::atomic<bool>& stop) {
    int l = i, r = (i + 1) % N;
    int first  = (i % 2 == 0) ? l : r;
    int second = (i % 2 == 0) ? r : l;
    while (!stop.load()) {
        observed_lock(i, first, 0);
        observed_lock(i, second, 1);
        g_state.meals[i]++;
        observed_unlock(i, second, 1);
        observed_unlock(i, first, 0);
    }
}

// FIX 3：trylock 回退
inline void philosopher_trylock(int i, std::atomic<bool>& stop) {
    int l = i, r = (i + 1) % N;
    while (!stop.load()) {
        observed_lock(i, l, 0);
        if (g_state.forks[r].try_lock()) {
            g_state.hold2[i].store(r);
            g_state.meals[i]++;
            g_state.hold2[i].store(-1);
            g_state.forks[r].unlock();
            observed_unlock(i, l, 0);
        } else {
            observed_unlock(i, l, 0);
            std::this_thread::sleep_for(std::chrono::microseconds(10));
        }
    }
}

}  // namespace mlab::lock
