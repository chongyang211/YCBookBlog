// box_ns — 迷你 docker run（Linux only）
// 文档：§19 盒 ⑪
#ifndef __linux__
#include "fmt_shim.hpp"
int main() { fmt::print("box_ns: Linux only. use Vagrantfile.\n"); return 0; }
#else

#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <sched.h>
#include <signal.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "fmt_shim.hpp"
#include <string>
#include <vector>

alignas(16) static char g_child_stack[1024 * 1024];

struct ChildArg {
    std::string rootfs;
    std::vector<std::string> cmd;
};

static int child_fn(void* a) {
    auto* arg = static_cast<ChildArg*>(a);
    if (::sethostname("mycontain", 9) < 0) perror("sethostname");
    if (::chroot(arg->rootfs.c_str()) < 0) {
        fmt::print(stderr, "chroot {}: {}\n", arg->rootfs, strerror(errno));
        return 1;
    }
    if (::chdir("/") < 0) { perror("chdir"); return 1; }
    ::mkdir("/proc", 0555);
    if (::mount("proc", "/proc", "proc",
                MS_NOSUID | MS_NOEXEC | MS_NODEV, nullptr) < 0) {
        perror("mount /proc"); return 1;
    }
    ::mkdir("/tmp", 01777);
    ::mount("tmpfs", "/tmp", "tmpfs", MS_NOSUID | MS_NODEV, "size=64m");

    std::vector<char*> argv;
    for (auto& s : arg->cmd) argv.push_back(const_cast<char*>(s.c_str()));
    argv.push_back(nullptr);
    fmt::print("[container] exec {}\n", arg->cmd[0]);
    ::execvp(argv[0], argv.data());
    perror("execvp");
    return 1;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        fmt::print("usage: {} <rootfs> [cmd...]\n", argv[0]);
        return 1;
    }
    if (::geteuid() != 0)
        fmt::print(stderr, "warn: run with sudo (CLONE_NEWPID/NEWNET need CAP_SYS_ADMIN)\n");

    ChildArg arg;
    arg.rootfs = argv[1];
    if (argc > 2) for (int i = 2; i < argc; ++i) arg.cmd.push_back(argv[i]);
    else arg.cmd = {"/bin/sh"};

    int flags = CLONE_NEWPID | CLONE_NEWNS | CLONE_NEWUTS |
                CLONE_NEWIPC | CLONE_NEWNET | SIGCHLD;
    pid_t pid = ::clone(child_fn, g_child_stack + sizeof(g_child_stack), flags, &arg);
    if (pid < 0) {
        perror("clone");
        if (errno == EPERM) fmt::print(stderr, "hint: sudo required\n");
        return 1;
    }
    fmt::print("[host] container pid={}\n", pid);
    int st = 0;
    ::waitpid(pid, &st, 0);
    fmt::print("[host] container exited (status={})\n", WEXITSTATUS(st));
    return 0;
}
#endif
