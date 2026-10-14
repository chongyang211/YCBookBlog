// box_proc — 进程状态机 + fork/exec/wait + BUG-1 僵尸复现
// 文档：§09 盒 ①
#include "repl.hpp"
#include "log.hpp"

#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include <atomic>
#include <string>
#include <vector>

struct Child { pid_t pid; std::string cmd; };
static std::vector<Child> g_children;
static std::atomic<int>   g_reaped{0};

static void sigchld_handler(int) {
    int saved_errno = errno;
    while (true) {
        int status;
        pid_t pid = ::waitpid(-1, &status, WNOHANG);
        if (pid <= 0) break;
        g_reaped.fetch_add(1);
        // async-signal-safe：只 write，不 fmt::print
        ::write(STDOUT_FILENO, ".", 1);
    }
    errno = saved_errno;
}

static void cmd_spawn(const std::vector<std::string>& args) {
    if (args.empty()) { LOG_ERR("usage: spawn <cmd> [args...]\n"); return; }
    pid_t pid = ::fork();
    if (pid < 0) { LOG_ERR("fork failed: {}\n", std::strerror(errno)); return; }
    if (pid == 0) {
        std::vector<char*> argv;
        for (auto& s : args) argv.push_back(const_cast<char*>(s.c_str()));
        argv.push_back(nullptr);
        ::execvp(argv[0], argv.data());
        ::fprintf(stderr, "[child] exec failed: %s\n", std::strerror(errno));
        ::_exit(127);
    }
    g_children.push_back({pid, args[0]});
    LOG_INFO("spawned pid={} cmd={}\n", pid, args[0]);
}

static void cmd_ps(const std::vector<std::string>&) {
    LOG_INFO("tracked children = {}, reaped = {}\n",
             g_children.size(), g_reaped.load());
    for (auto& c : g_children)
        fmt::print("  pid={} cmd={}\n", c.pid, c.cmd);
}

static void cmd_kill(const std::vector<std::string>& a) {
    if (a.empty()) { LOG_ERR("usage: kill <pid>\n"); return; }
    pid_t p = (pid_t)std::stoi(a[0]);
    if (::kill(p, SIGTERM) == 0) LOG_INFO("sent SIGTERM to {}\n", p);
    else                         LOG_ERR("kill {}: {}\n", p, std::strerror(errno));
}

// 🔥 BUG-1 现场：N 次 fork 不 wait（若 SIGCHLD handler 已装则会被回收）
static void cmd_zombie(const std::vector<std::string>& a) {
    int n = a.empty() ? 10 : std::stoi(a[0]);
    for (int i = 0; i < n; ++i) {
        pid_t pid = ::fork();
        if (pid == 0) { ::_exit(0); }
        if (pid > 0) g_children.push_back({pid, "zombie"});
    }
    LOG_WARN("spawned {} tiny children; check `ps -ef | grep defunct`\n", n);
}

int main(int argc, char** argv) {
    bool fix = false;
    for (int i = 1; i < argc; ++i) if (std::string(argv[i]) == "--fix") fix = true;

    if (fix) {
        struct sigaction sa{};
        sa.sa_handler = sigchld_handler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
        ::sigaction(SIGCHLD, &sa, nullptr);
        LOG_INFO("SIGCHLD handler installed (--fix mode). zombies will be reaped.\n");
    } else {
        LOG_WARN("BUG mode: no SIGCHLD handler. use `zombie 10` + `ps -ef | grep defunct`.\n");
        LOG_WARN("add --fix to install reaper.\n");
    }

    mlab::Repl repl;
    repl.reg("spawn",  "spawn <cmd> [args]  : fork + exec",   cmd_spawn);
    repl.reg("ps",     "list tracked children",               cmd_ps);
    repl.reg("kill",   "kill <pid>",                          cmd_kill);
    repl.reg("zombie", "zombie <n>  : fork n children; no wait (BUG-1)",
             cmd_zombie);
    LOG_INFO("box_proc ready. type `help`.\n");
    return repl.run();
}
