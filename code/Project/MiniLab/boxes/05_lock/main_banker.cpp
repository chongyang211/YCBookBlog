// main_banker — 银行家算法 CLI (OSTEP ch32 预置样例)
#include "repl.hpp"
#include "fmt_shim.hpp"
#include <sstream>
#include <string>
#include <vector>

struct Banker {
    int n = 0, m = 0;
    std::vector<int> avail;
    std::vector<std::vector<int>> maxv, alloc, need;

    void recompute_need() {
        need.assign(n, std::vector<int>(m));
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < m; ++j)
                need[i][j] = maxv[i][j] - alloc[i][j];
    }

    bool is_safe_after(int pid, const std::vector<int>& req,
                       std::vector<int>* order_out) const {
        auto av = avail; auto al = alloc; auto nd = need;
        if (pid >= 0) {
            for (int j = 0; j < m; ++j) {
                if (req[j] > nd[pid][j] || req[j] > av[j]) return false;
                av[j] -= req[j]; al[pid][j] += req[j]; nd[pid][j] -= req[j];
            }
        }
        std::vector<bool> fin(n, false);
        std::vector<int> order;
        for (int k = 0; k < n; ++k) {
            bool prog = false;
            for (int i = 0; i < n; ++i) {
                if (fin[i]) continue;
                bool ok = true;
                for (int j = 0; j < m; ++j)
                    if (nd[i][j] > av[j]) { ok = false; break; }
                if (ok) {
                    for (int j = 0; j < m; ++j) av[j] += al[i][j];
                    fin[i] = true; order.push_back(i); prog = true;
                }
            }
            if (!prog) return false;
        }
        if (order_out) *order_out = order;
        return true;
    }
};

static Banker bk;

static std::vector<int> parse_vec(const std::string& s) {
    std::vector<int> r; std::stringstream ss(s); std::string t;
    while (std::getline(ss, t, ',')) r.push_back(std::stoi(t));
    return r;
}

static void preset_ostep() {
    bk.n = 5; bk.m = 3;
    bk.avail = {3, 3, 2};
    bk.maxv  = {{7,5,3},{3,2,2},{9,0,2},{2,2,2},{4,3,3}};
    bk.alloc = {{0,1,0},{2,0,0},{3,0,2},{2,1,1},{0,0,2}};
    bk.recompute_need();
    fmt::print("preset loaded: OSTEP ch32 (5 procs x 3 resources)\n");
}

int main() {
    mlab::Repl r;
    preset_ostep();
    r.reg("show", "show tables", [](auto&){
        fmt::print("Available = [{}]\n", fmt::join(bk.avail, ","));
        for (int i = 0; i < bk.n; ++i)
            fmt::print("P{}: Max=[{}] Alloc=[{}] Need=[{}]\n",
                       i, fmt::join(bk.maxv[i], ","),
                       fmt::join(bk.alloc[i], ","),
                       fmt::join(bk.need[i], ","));
    });
    r.reg("safe", "check current state", [](auto&){
        std::vector<int> order;
        bool ok = bk.is_safe_after(-1, {}, &order);
        if (ok) {
            fmt::print("SAFE. order:");
            for (int p : order) fmt::print(" P{}", p);
            fmt::print("\n");
        } else fmt::print("UNSAFE\n");
    });
    r.reg("req", "req <pid> <v1,v2,v3>", [](auto& a){
        if (a.size() < 2) { fmt::print("usage\n"); return; }
        int pid = std::stoi(a[0]);
        auto req = parse_vec(a[1]);
        std::vector<int> order;
        if (!bk.is_safe_after(pid, req, &order)) {
            fmt::print("[REFUSE] P{} + [{}] would be unsafe\n",
                       pid, fmt::join(req, ","));
            return;
        }
        for (int j = 0; j < bk.m; ++j) {
            bk.avail[j] -= req[j];
            bk.alloc[pid][j] += req[j];
            bk.need[pid][j]  -= req[j];
        }
        fmt::print("[GRANT ] P{} got [{}], safe order:", pid, fmt::join(req, ","));
        for (int p : order) fmt::print(" P{}", p);
        fmt::print("\n");
    });
    return r.run();
}
