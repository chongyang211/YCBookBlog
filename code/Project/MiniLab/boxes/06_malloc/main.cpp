// box_malloc — REPL: alloc / free / stat / stress / reap
// 文档：§14 盒 ⑥
#include "allocator.hpp"
#include "repl.hpp"
#include "log.hpp"
#include "timer.hpp"
#include "fmt_shim.hpp"
#include <sstream>
#include <unordered_map>
#include <vector>

using namespace mlab::mm;

static Allocator alloc_;
static std::unordered_map<int, void*> handles_;
static int next_id_ = 1;

static void cmd_alloc(const std::vector<std::string>& a) {
    int sz = a.empty() ? 32 : std::atoi(a[0].c_str());
    void* p = alloc_.alloc(sz);
    if (!p) { LOG_ERR("alloc {} failed\n", sz); return; }
    int id = next_id_++;
    handles_[id] = p;
    LOG_INFO("id={} ptr={} size={}\n", id, p, sz);
}
static void cmd_free(const std::vector<std::string>& a) {
    if (a.empty()) { LOG_ERR("usage: free <id>\n"); return; }
    int id = std::atoi(a[0].c_str());
    auto it = handles_.find(id); if (it == handles_.end()) { LOG_ERR("no such id\n"); return; }
    alloc_.free(it->second); handles_.erase(it);
    LOG_OK("freed id={}\n", id);
}

static void cmd_stat(const std::vector<std::string>&) {
    auto bf = alloc_.buddy_counts();
    fmt::print("=== Buddy ===\n");
    for (int o = 0; o <= MAX_ORDER; ++o)
        if (bf[o]) fmt::print("  order={} (={}KB) free_blocks={}\n",
                              o, (1ULL << o) * PAGE_BYTES / 1024, bf[o]);
    fmt::print("  total_free={}KB\n", alloc_.buddy_free_bytes() / 1024);
    fmt::print("=== Slab ===\n");
    for (auto& s : alloc_.slab_stats())
        fmt::print("  cls={}B pages={} in_use={} cached_obj={}\n",
                   s.obj_size, s.pages, s.in_use, s.cached);
    fmt::print("=== User === requested={}B live={}\n",
               alloc_.user_in_use(), handles_.size());
    long pb = 0, cb = 0;
    for (auto& s : alloc_.slab_stats()) {
        pb += (long)s.pages * PAGE_BYTES;
        cb += (long)s.cached * s.obj_size;
    }
    double frag = pb > 0 ? 100.0 * cb / pb : 0;
    fmt::print("  slab_internal_frag={:.1f}%\n", frag);
}

static void cmd_stress(const std::vector<std::string>& a) {
    if (a.size() < 2) { LOG_ERR("usage: stress <sz1,sz2,...> <N> [keep_every=128]\n"); return; }
    std::vector<int> sizes;
    { std::stringstream ss(a[0]); std::string tok;
      while (std::getline(ss, tok, ',')) sizes.push_back(std::atoi(tok.c_str())); }
    int N = std::atoi(a[1].c_str());
    int keep = a.size() > 2 ? std::atoi(a[2].c_str()) : 128;

    std::vector<void*> keeping;
    mlab::Timer t;
    for (int i = 0; i < N; ++i) {
        int sz = sizes[i % sizes.size()];
        void* p = alloc_.alloc(sz);
        if (!p) { LOG_ERR("OOM at i={}\n", i); break; }
        if (i % keep == 0) keeping.push_back(p);
        else                alloc_.free(p);
    }
    LOG_OK("stress N={} dt={:.1f}ms keep={}\n", N, t.elapsed_ms(), keeping.size());
    for (void* p : keeping) alloc_.free(p);
}

static void cmd_reap(const std::vector<std::string>&) {
    int n = alloc_.reap();
    LOG_OK("reaped {} empty slab pages\n", n);
}

static void cmd_bench(const std::vector<std::string>& a) {
    int N = a.empty() ? 1000000 : std::atoi(a[0].c_str());
    std::vector<void*> ps(N);
    mlab::Timer t;
    for (int i = 0; i < N; ++i) ps[i] = alloc_.alloc(32);
    double a_ns = t.elapsed_ns() / N;
    mlab::Timer t2;
    for (int i = 0; i < N; ++i) alloc_.free(ps[i]);
    double f_ns = t2.elapsed_ns() / N;
    LOG_OK("alloc={:.1f}ns free={:.1f}ns (N={})\n", a_ns, f_ns, N);
}

int main() {
    mlab::Repl r;
    r.reg("alloc",  "alloc <size>",             cmd_alloc);
    r.reg("free",   "free <id>",                cmd_free);
    r.reg("stat",   "allocator state",          cmd_stat);
    r.reg("stress", "stress <sz,sz,..> <N>",    cmd_stress);
    r.reg("reap",   "reap empty slab pages",    cmd_reap);
    r.reg("bench",  "bench <N=1M>",             cmd_bench);
    LOG_INFO("box_malloc ready. arena=16MB.\n");
    return r.run();
}
