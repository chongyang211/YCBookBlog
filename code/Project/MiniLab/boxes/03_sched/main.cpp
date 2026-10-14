// box_sched — 五种调度器 benchmark
// 文档：§11 盒 ③
#include "scheduler.hpp"
#include "task.hpp"
#include "csv.hpp"
#include "log.hpp"
#include "fmt_shim.hpp"

static void run_and_report(auto& s, std::vector<mlab::sched::Task> t,
                           const std::string& name, mlab::CsvWriter& csv,
                           const std::string& q = "-") {
    s.run(t);
    fmt::print("\n=== {} ===\n", name);
    s.print_gantt();
    double tt = 0, tr = 0, tw = 0;
    for (auto& x : t) { tt += x.turnaround(); tr += x.response(); tw += x.wait_total; }
    double n = t.size();
    fmt::print("  avg_turnaround={:.2f} avg_response={:.2f} avg_wait={:.2f}\n",
               tt / n, tr / n, tw / n);
    csv.row(name, q,
            fmt::format("{:.2f}", tt / n),
            fmt::format("{:.2f}", tr / n),
            fmt::format("{:.2f}", tw / n));
}

int main(int argc, char** argv) {
    std::string file = argc > 1 ? argv[1] : "demo/sched/mixed.task";
    std::string algo = argc > 2 ? argv[2] : "all";
    auto base = mlab::sched::load_tasks(file);
    LOG_INFO("loaded {} tasks from {}\n", base.size(), file);

    mlab::CsvWriter csv("out/csv/box_sched.csv",
        {"algo", "quantum", "avg_turnaround", "avg_response", "avg_wait"});

    if (algo == "all" || algo == "fcfs") { mlab::sched::FCFS s; run_and_report(s, base, "FCFS", csv); }
    if (algo == "all" || algo == "sjf")  { mlab::sched::SJF  s; run_and_report(s, base, "SJF",  csv); }
    if (algo == "all" || algo == "rr")   { mlab::sched::RR   s(2); run_and_report(s, base, "RR(q=2)", csv, "2"); }
    if (algo == "all" || algo == "mlfq") { mlab::sched::MLFQ s; run_and_report(s, base, "MLFQ", csv); }
    if (algo == "all" || algo == "cfs")  { mlab::sched::CFS  s; run_and_report(s, base, "CFS",  csv); }
    return 0;
}
