// scheduler.hpp — 五种调度器（FCFS/SJF/RR/MLFQ/CFS）
#pragma once
#include "task.hpp"
#include <algorithm>
#include "fmt_shim.hpp"
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace mlab::sched {

struct GanttSegment { int start; int dur; std::string pid; };

template <class Derived>
class SchedulerBase {
public:
    void run(std::vector<Task>& tasks) {
        for (auto& t : tasks) t.reset();
        int now = 0, done = 0;
        Task* running = nullptr;
        int run_start = 0;

        auto commit = [&](int end) {
            if (end > run_start)
                gantt_.push_back({run_start, end - run_start,
                                  running ? running->pid : std::string("IDLE")});
        };

        while (done < (int)tasks.size()) {
            Task* next = static_cast<Derived*>(this)->pick_next(now, tasks);

            if (next != running) {
                commit(now);
                run_start = now;
                running = next;
                if (running && running->first_run_at < 0)
                    running->first_run_at = now;
            }
            if (running) {
                running->remaining -= 1;
                if (running->remaining == 0) {
                    running->finish_at = now + 1;
                    done++;
                    commit(now + 1);
                    run_start = now + 1;
                    running = nullptr;
                }
            }
            for (auto& t : tasks) {
                if (&t == running) continue;
                if (t.arrive <= now && t.remaining > 0) t.wait_total++;
            }
            if (++now > 10000) { fmt::print("WARN: timeout\n"); break; }
        }
    }

    void print_gantt() const {
        if (gantt_.empty()) return;
        int end = gantt_.back().start + gantt_.back().dur;
        std::vector<std::string> pids;
        for (auto& s : gantt_)
            if (s.pid != "IDLE" &&
                std::find(pids.begin(), pids.end(), s.pid) == pids.end())
                pids.push_back(s.pid);
        for (auto& p : pids) {
            fmt::print("  {:<6}: ", p);
            for (int t = 0; t < end; ++t) {
                bool hit = false;
                for (auto& s : gantt_)
                    if (s.pid == p && t >= s.start && t < s.start + s.dur) {
                        hit = true; break;
                    }
                fmt::print("{}", hit ? "#" : " ");
            }
            fmt::print("\n");
        }
        fmt::print("  time  : ");
        for (int t = 0; t < end; ++t)
            fmt::print("{}", (t % 10 == 0) ? ('0' + (t / 10) % 10) : '.');
        fmt::print("\n");
    }

protected:
    std::vector<GanttSegment> gantt_;
};

// ===== FCFS =====
class FCFS : public SchedulerBase<FCFS> {
public:
    Task* pick_next(int now, std::vector<Task>& tasks) {
        // 非抢占：有运行中就继续
        for (auto& t : tasks)
            if (t.arrive <= now && t.remaining > 0 &&
                t.first_run_at >= 0 && t.finish_at < 0)
                return &t;
        Task* best = nullptr;
        for (auto& t : tasks)
            if (t.arrive <= now && t.remaining > 0)
                if (!best || t.arrive < best->arrive) best = &t;
        return best;
    }
};

// ===== SJF 非抢占 =====
class SJF : public SchedulerBase<SJF> {
public:
    Task* pick_next(int now, std::vector<Task>& tasks) {
        for (auto& t : tasks)
            if (t.arrive <= now && t.remaining > 0 &&
                t.first_run_at >= 0 && t.finish_at < 0)
                return &t;
        Task* best = nullptr;
        for (auto& t : tasks)
            if (t.arrive <= now && t.remaining > 0)
                if (!best || t.remaining < best->remaining) best = &t;
        return best;
    }
};

// ===== RR =====
class RR : public SchedulerBase<RR> {
public:
    explicit RR(int quantum_ms) : q_(quantum_ms) {}
    Task* pick_next(int now, std::vector<Task>& tasks) {
        if (last_ && last_->remaining > 0 && used_ < q_) {
            used_++;
            return last_;
        }
        int n = tasks.size();
        for (int i = 1; i <= n; ++i) {
            int idx = (last_idx_ + i) % n;
            auto& t = tasks[idx];
            if (t.arrive <= now && t.remaining > 0) {
                last_ = &t; last_idx_ = idx; used_ = 1;
                return &t;
            }
        }
        last_ = nullptr;
        return nullptr;
    }
private:
    int q_;
    Task* last_ = nullptr;
    int last_idx_ = -1;
    int used_ = 0;
};

// ===== MLFQ =====
class MLFQ : public SchedulerBase<MLFQ> {
public:
    static constexpr int LEVELS = 4;
    static constexpr int BOOST_PERIOD = 50;
    const int QUANTUM[LEVELS] = {2, 4, 8, 16};

    Task* pick_next(int now, std::vector<Task>& tasks) {
        if (now > 0 && now % BOOST_PERIOD == 0)
            for (auto& [k, v] : level_) v = 0;
        for (auto& t : tasks)
            if (t.arrive <= now && t.remaining > 0 && !level_.count(t.pid))
                level_[t.pid] = 0;

        if (last_ && last_->remaining > 0) {
            if (used_ < QUANTUM[level_[last_->pid]]) { used_++; return last_; }
            if (level_[last_->pid] < LEVELS - 1) level_[last_->pid]++;
        }
        for (int lv = 0; lv < LEVELS; ++lv) {
            Task* best = nullptr;
            for (auto& t : tasks)
                if (t.arrive <= now && t.remaining > 0 && level_[t.pid] == lv)
                    if (!best || t.arrive < best->arrive) best = &t;
            if (best) { last_ = best; used_ = 1; return best; }
        }
        last_ = nullptr;
        return nullptr;
    }

private:
    std::unordered_map<std::string, int> level_;
    Task* last_ = nullptr;
    int used_ = 0;
};

// ===== CFS =====
class CFS : public SchedulerBase<CFS> {
public:
    using VR = long long;
    static constexpr int WEIGHT[40] = {
        88761,71755,56483,46273,36291, 29154,23254,18705,14949,11916,
         9548, 7620, 6100, 4904, 3906,  3121, 2501, 1991, 1586, 1277,
         1024,  820,  655,  526,  423,   335,  272,  215,  172,  137,
          110,   87,   70,   56,   45,    36,   29,   23,   18,   15,
    };
    static int weight_of(int nice) {
        if (nice < -20) nice = -20;
        if (nice >  19) nice =  19;
        return WEIGHT[nice + 20];
    }

    Task* pick_next(int now, std::vector<Task>& tasks) {
        for (auto& t : tasks) {
            if (t.arrive <= now && t.remaining > 0 && !inserted_.count(&t)) {
                vruntime_[&t] = min_vr_;
                rbt_.insert({vruntime_[&t], &t});
                inserted_.insert(&t);
            }
        }
        if (last_) {
            VR delta = 1000LL * 1024 / weight_of(last_->nice);
            vruntime_[last_] += delta;
            rbt_.erase({last_vr_, last_});
            if (last_->remaining > 0) rbt_.insert({vruntime_[last_], last_});
        }
        if (rbt_.empty()) { last_ = nullptr; return nullptr; }
        auto it = rbt_.begin();
        last_    = it->second;
        last_vr_ = it->first;
        min_vr_  = last_vr_;
        return last_;
    }

private:
    struct Cmp {
        bool operator()(std::pair<VR, Task*> a, std::pair<VR, Task*> b) const {
            if (a.first != b.first) return a.first < b.first;
            return a.second < b.second;
        }
    };
    std::set<std::pair<VR, Task*>, Cmp> rbt_;
    std::unordered_map<Task*, VR> vruntime_;
    std::set<Task*> inserted_;
    Task* last_ = nullptr;
    VR last_vr_ = 0, min_vr_ = 0;
};

}  // namespace mlab::sched
