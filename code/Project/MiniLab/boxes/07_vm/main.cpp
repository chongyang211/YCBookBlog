// box_vm — 五种页替换 benchmark + Belady 异常复现
// 文档：§15 盒 ⑦
#include "replacer.hpp"
#include "trace.hpp"
#include "csv.hpp"
#include "log.hpp"
#include "fmt_shim.hpp"
#include <memory>
#include <string>
#include <vector>

using namespace mlab::vm;

int main(int argc, char** argv) {
    std::string kind = argc > 1 ? argv[1] : "belady";
    Trace t;
    if      (kind == "belady") t = belady_classic();
    else if (kind == "seq")    t = sequential(argc > 2 ? std::atoi(argv[2]) : 5, 3);
    else if (kind == "ws")     t = working_set(argc > 2 ? std::atoi(argv[2]) : 10,
                                               argc > 3 ? std::atoi(argv[3]) : 100,
                                               argc > 4 ? std::atoi(argv[4]) : 1000);
    else if (kind == "file")   t = load_trace(argv[2]);
    else { LOG_ERR("usage: box_vm belady|seq N|ws hot total len|file path\n"); return 1; }

    LOG_INFO("trace: kind={} len={}\n", kind, t.size());

    mlab::CsvWriter csv("out/csv/box_vm.csv",
        {"algo","frames","faults","hits","total","hit_rate"});

    int max_frames = (kind == "belady") ? 6 : 20;
    for (int f = 3; f <= max_frames; ++f) {
        std::vector<std::unique_ptr<Replacer>> algs;
        algs.push_back(std::make_unique<FIFO>(f));
        algs.push_back(std::make_unique<LRU>(f));
        algs.push_back(std::make_unique<Clock>(f));
        algs.push_back(std::make_unique<EnhClock>(f));
        algs.push_back(std::make_unique<OPT>(f, t));
        for (auto& a : algs) {
            for (int p : t) a->access(p);
            fmt::print("  frames={:2d} {:9s} faults={:4d} hit_rate={:.3f}\n",
                       f, a->name(), a->faults(), a->hit_rate());
            csv.row(a->name(), std::to_string(f),
                    std::to_string(a->faults()),
                    std::to_string(a->hits()),
                    std::to_string(a->total()),
                    fmt::format("{:.4f}", a->hit_rate()));
        }
        fmt::print("\n");
    }
    return 0;
}
