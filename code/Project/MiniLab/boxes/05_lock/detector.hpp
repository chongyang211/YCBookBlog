// detector.hpp — 资源分配图 DFS 死锁检测（三色法）
#pragma once
#include "philosopher.hpp"
#include <algorithm>
#include <array>
#include <functional>
#include <optional>
#include <vector>

namespace mlab::lock {

inline std::optional<std::vector<int>> detect_deadlock() {
    std::array<int, N> owner;
    std::array<std::vector<int>, N> waiters;
    owner.fill(-1);
    for (int p = 0; p < N; ++p) {
        int h1 = g_state.hold[p].load();
        int h2 = g_state.hold2[p].load();
        if (h1 >= 0) owner[h1] = p;
        if (h2 >= 0) owner[h2] = p;
    }
    for (int p = 0; p < N; ++p) {
        int w = g_state.waiting[p].load();
        if (w >= 0) waiters[w].push_back(p);
    }
    std::array<std::vector<int>, N> adj;
    for (int f = 0; f < N; ++f) {
        if (owner[f] < 0) continue;
        for (int wp : waiters[f]) adj[owner[f]].push_back(wp);
    }
    enum { WHITE, GRAY, BLACK };
    std::array<int, N> color; color.fill(WHITE);
    std::vector<int> stack, cycle;
    std::function<bool(int)> dfs = [&](int u) -> bool {
        color[u] = GRAY; stack.push_back(u);
        for (int v : adj[u]) {
            if (color[v] == GRAY) {
                auto it = std::find(stack.begin(), stack.end(), v);
                cycle.assign(it, stack.end());
                return true;
            }
            if (color[v] == WHITE && dfs(v)) return true;
        }
        color[u] = BLACK; stack.pop_back();
        return false;
    };
    for (int i = 0; i < N; ++i)
        if (color[i] == WHITE && dfs(i)) return cycle;
    return std::nullopt;
}

}  // namespace mlab::lock
