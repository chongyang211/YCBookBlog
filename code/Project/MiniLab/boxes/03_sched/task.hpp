// task.hpp — 调度器任务模型
#pragma once
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace mlab::sched {

struct Task {
    std::string pid;
    int arrive = 0;
    int burst  = 0;
    int nice   = 0;

    // runtime
    int remaining     = 0;
    int first_run_at  = -1;
    int finish_at     = -1;
    int wait_total    = 0;

    void reset() {
        remaining = burst;
        first_run_at = -1;
        finish_at = -1;
        wait_total = 0;
    }
    int response()    const { return first_run_at - arrive; }
    int turnaround()  const { return finish_at    - arrive; }
};

inline std::vector<Task> load_tasks(const std::string& path) {
    std::ifstream f(path);
    if (!f) throw std::runtime_error("open " + path);
    std::vector<Task> ts;
    std::string line;
    while (std::getline(f, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::stringstream ss(line);
        Task t;
        ss >> t.pid >> t.arrive >> t.burst;
        if (!(ss >> t.nice)) t.nice = 0;
        t.reset();
        ts.push_back(t);
    }
    return ts;
}

}  // namespace mlab::sched
