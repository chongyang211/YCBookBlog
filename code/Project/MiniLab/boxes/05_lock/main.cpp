// box_lock — mode: bug|order|parity|trylock
#include "philosopher.hpp"
#include "detector.hpp"
#include "log.hpp"
#include "csv.hpp"
#include <chrono>
#include <cstdlib>
#include "fmt_shim.hpp"
#include <string>
#include <thread>
#include <vector>

using namespace mlab::lock;
using Clock = std::chrono::steady_clock;

int main(int argc, char** argv) {
    std::string mode = argc > 1 ? argv[1] : "order";
    int dur = argc > 2 ? std::atoi(argv[2]) : 3;

    void (*fn)(int, std::atomic<bool>&) = nullptr;
    if      (mode == "bug")     fn = philosopher_bug;
    else if (mode == "order")   fn = philosopher_order;
    else if (mode == "parity")  fn = philosopher_parity;
    else if (mode == "trylock") fn = philosopher_trylock;
    else { LOG_ERR("mode: bug|order|parity|trylock\n"); return 1; }

    std::atomic<bool> stop{false};
    std::atomic<bool> deadlock{false};
    std::vector<std::thread> phils;
    for (int i = 0; i < N; ++i) phils.emplace_back(fn, i, std::ref(stop));

    std::thread det([&]{
        auto t0 = Clock::now();
        while (!stop.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            if (auto cyc = detect_deadlock()) {
                auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                    Clock::now() - t0).count();
                LOG_ERR("DEADLOCK DETECTED after {}ms\n", ms);
                for (size_t i = 0; i < cyc->size(); ++i) {
                    int p = (*cyc)[i], np = (*cyc)[(i + 1) % cyc->size()];
                    int wf = g_state.waiting[p].load();
                    fmt::print("  P{} --[wants f{}]--> P{}\n", p, wf, np);
                }
                deadlock.store(true); stop.store(true);
                std::_Exit(2);
            }
        }
    });

    auto t0 = Clock::now();
    std::this_thread::sleep_for(std::chrono::seconds(dur));
    stop.store(true);
    for (auto& t : phils) t.join();
    det.join();
    double sec = std::chrono::duration<double>(Clock::now() - t0).count();

    long total = 0;
    for (int i = 0; i < N; ++i) total += g_state.meals[i].load();
    LOG_OK("mode={} dur={:.1f}s meals={} qps={:.0f} deadlock={}\n",
           mode, sec, total, total / sec, deadlock.load());

    mlab::CsvWriter csv("out/csv/box_lock.csv",
        {"mode","duration_s","meals","qps","deadlock"}, true);
    csv.row(mode, fmt::format("{:.2f}", sec), std::to_string(total),
            fmt::format("{:.1f}", total / sec),
            deadlock.load() ? "yes" : "no");
    return 0;
}
