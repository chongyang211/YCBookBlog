// mycgrp — cgroup v2 CPU/Memory 控制工具
// 文档：§20 盒 ⑫
#ifndef __linux__
#include "fmt_shim.hpp"
int main() { fmt::print("mycgrp: Linux only.\n"); return 0; }
#else

#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include <chrono>
#include <filesystem>
#include "fmt_shim.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace fs = std::filesystem;
static const char* CG_ROOT = "/sys/fs/cgroup";

static void wf(const fs::path& p, const std::string& s) {
    std::ofstream f(p);
    if (!f) throw std::runtime_error("open " + p.string() + ": " + strerror(errno));
    f << s;
    if (!f) throw std::runtime_error("write " + p.string());
}

static std::string rf(const fs::path& p) {
    std::ifstream f(p);
    if (!f) return "";
    return std::string((std::istreambuf_iterator<char>(f)), {});
}

static void check_v2() {
    if (!fs::exists(fs::path(CG_ROOT) / "cgroup.controllers"))
        throw std::runtime_error("cgroup v2 not available");
}

static fs::path gp(const std::string& n) { return fs::path(CG_ROOT) / n; }

static void cmd_create(const std::string& n) {
    check_v2();
    auto p = gp(n);
    if (fs::exists(p)) { fmt::print("already exists: {}\n", p.string()); return; }
    if (::mkdir(p.c_str(), 0755) < 0) throw std::runtime_error("mkdir");
    try { wf(fs::path(CG_ROOT) / "cgroup.subtree_control", "+cpu +memory"); } catch (...) {}
    fmt::print("created {}\n", p.string());
}

static void cmd_rm(const std::string& n) {
    auto p = gp(n);
    auto procs = rf(p / "cgroup.procs");
    std::stringstream ss(procs); std::string pid;
    while (std::getline(ss, pid))
        if (!pid.empty())
            try { wf(fs::path(CG_ROOT) / "cgroup.procs", pid); } catch (...) {}
    if (::rmdir(p.c_str()) < 0) throw std::runtime_error("rmdir");
    fmt::print("removed {}\n", p.string());
}

static void cmd_attach(const std::string& n, pid_t pid) {
    wf(gp(n) / "cgroup.procs", std::to_string(pid));
    fmt::print("attached pid={} to {}\n", pid, n);
}

static void cmd_limit_cpu(const std::string& n, long q, long p) {
    auto s = fmt::format("{} {}", q, p);
    wf(gp(n) / "cpu.max", s);
    fmt::print("cpu.max = {}\n", s);
}

static long parse_size(const std::string& s) {
    char* end; double v = std::strtod(s.c_str(), &end);
    char c = *end; long m = 1;
    switch (c) {
        case 'K': case 'k': m = 1024; break;
        case 'M': case 'm': m = 1024 * 1024; break;
        case 'G': case 'g': m = 1024L * 1024 * 1024; break;
        case 0: break;
        default: throw std::runtime_error("bad size: " + s);
    }
    return (long)(v * m);
}

static void cmd_limit_mem(const std::string& n, const std::string& sz) {
    long b = parse_size(sz);
    wf(gp(n) / "memory.max", std::to_string(b));
    try { wf(gp(n) / "memory.swap.max", "0"); } catch (...) {}
    fmt::print("memory.max = {}B ({}MB), swap=0\n", b, b / 1024 / 1024);
}

static void cpu_stress(int sec) {
    auto t0 = std::chrono::steady_clock::now();
    volatile long x = 0;
    while (std::chrono::steady_clock::now() - t0 < std::chrono::seconds(sec))
        for (int i = 0; i < 10000000; ++i) x += i;
    fmt::print("cpu_stress done (x={})\n", (long)x);
}

static void mem_stress(int MB) {
    long total = (long)MB * 1024 * 1024;
    constexpr int PAGE = 4096;
    char* p = (char*)malloc(total);
    if (!p) { fmt::print("malloc failed\n"); return; }
    for (long off = 0; off < total; off += PAGE) {
        p[off] = (char)(off & 0xff);
        if ((off % (16L * 1024 * 1024)) == 0)
            fmt::print("touched {}MB\n", off / 1024 / 1024);
    }
    fmt::print("mem_stress ok ({}MB)\n", MB);
    free(p);
}

static void cmd_stat(const std::string& n) {
    auto p = gp(n);
    fmt::print("=== cgroup: {} ===\n", n);
    fmt::print("cpu.max       : {}", rf(p / "cpu.max"));
    fmt::print("cpu.stat      :\n{}", rf(p / "cpu.stat"));
    fmt::print("memory.max    : {}", rf(p / "memory.max"));
    fmt::print("memory.current: {}", rf(p / "memory.current"));
    std::ifstream f(p / "memory.stat"); std::string l;
    fmt::print("memory.stat (key lines):\n");
    while (std::getline(f, l))
        if (l.rfind("anon ", 0) == 0 || l.rfind("file ", 0) == 0 ||
            l.rfind("kernel ", 0) == 0 || l.rfind("sock ", 0) == 0)
            fmt::print("  {}\n", l);
    fmt::print("procs         :\n{}", rf(p / "cgroup.procs"));
}

int main(int argc, char** argv) {
    if (argc < 2) {
        fmt::print(
            "usage:\n"
            "  {0} create <name>\n"
            "  {0} rm <name>\n"
            "  {0} attach <name> <pid>\n"
            "  {0} limit-cpu <name> <quota_us> <period_us>\n"
            "  {0} limit-mem <name> <bytes e.g. 128M>\n"
            "  {0} stat <name>\n"
            "  {0} cpu-stress <seconds>\n"
            "  {0} mem-stress <mb>\n",
            argv[0]);
        return 1;
    }
    try {
        std::string c = argv[1];
        if      (c == "create")      cmd_create(argv[2]);
        else if (c == "rm")          cmd_rm(argv[2]);
        else if (c == "attach")      cmd_attach(argv[2], std::atoi(argv[3]));
        else if (c == "limit-cpu")   cmd_limit_cpu(argv[2], std::atol(argv[3]), std::atol(argv[4]));
        else if (c == "limit-mem")   cmd_limit_mem(argv[2], argv[3]);
        else if (c == "stat")        cmd_stat(argv[2]);
        else if (c == "cpu-stress")  cpu_stress(std::atoi(argv[2]));
        else if (c == "mem-stress")  mem_stress(std::atoi(argv[2]));
        else { fmt::print("unknown cmd\n"); return 1; }
    } catch (const std::exception& e) {
        fmt::print(stderr, "error: {}\n", e.what());
        return 1;
    }
    return 0;
}
#endif
