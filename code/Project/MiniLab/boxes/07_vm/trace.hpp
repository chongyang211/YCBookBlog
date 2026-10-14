// trace.hpp — 页访问 trace
#pragma once
#include <fstream>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace mlab::vm {

using Trace = std::vector<int>;

inline Trace load_trace(const std::string& path) {
    std::ifstream f(path);
    if (!f) throw std::runtime_error("open " + path);
    Trace r;
    std::string line;
    while (std::getline(f, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::stringstream ss(line);
        int v; while (ss >> v) r.push_back(v);
    }
    return r;
}

inline Trace belady_classic() { return {1,2,3,4,1,2,5,1,2,3,4,5}; }

inline Trace sequential(int n_pages, int rounds = 3) {
    Trace t;
    for (int r = 0; r < rounds; ++r)
        for (int i = 1; i <= n_pages; ++i) t.push_back(i);
    return t;
}

inline Trace working_set(int hot, int total, int len, uint32_t seed = 42) {
    std::mt19937 rng(seed);
    std::uniform_real_distribution<> p(0, 1);
    std::uniform_int_distribution<> hp(1, hot);
    std::uniform_int_distribution<> cp(hot + 1, total);
    Trace t;
    for (int i = 0; i < len; ++i)
        t.push_back(p(rng) < 0.8 ? hp(rng) : cp(rng));
    return t;
}

}  // namespace mlab::vm
