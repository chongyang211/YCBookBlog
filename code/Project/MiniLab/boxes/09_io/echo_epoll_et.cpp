// echo_epoll_et — BUG/FIX 两模式 ET
// 用法：./echo_epoll_et <port>       → BUG 版（只读一次）
//      ./echo_epoll_et <port> --fix  → FIX 版（while 到 EAGAIN）
#include "common.hpp"
#ifndef __linux__
int main() { fmt::print("epoll: Linux only\n"); return 0; }
#else
#include <sys/epoll.h>
#include <string>
#include <vector>

using namespace mlab::io;

static bool g_fix = false;

static void handle_bug(int fd) {
    char buf[128];
    ssize_t r = ::read(fd, buf, sizeof(buf));
    if (r > 0) safe_write(fd, buf, r);
    else if (r == 0 || (r < 0 && errno != EAGAIN)) ::close(fd);
}

static void handle_fix(int fd) {
    while (true) {
        char buf[128];
        ssize_t r = ::read(fd, buf, sizeof(buf));
        if (r > 0) { safe_write(fd, buf, r); continue; }
        if (r == 0) { ::close(fd); return; }
        if (errno == EINTR) continue;
        if (errno == EAGAIN || errno == EWOULDBLOCK) return;
        ::close(fd); return;
    }
}

int main(int argc, char** argv) {
    int port = argc > 1 ? std::atoi(argv[1]) : 9004;
    for (int i = 2; i < argc; ++i) if (std::string(argv[i]) == "--fix") g_fix = true;
    ignore_sigpipe();
    int lfd = make_listener(port);
    int ep  = ::epoll_create1(EPOLL_CLOEXEC);
    epoll_event e{}; e.events = EPOLLIN; e.data.fd = lfd;
    ::epoll_ctl(ep, EPOLL_CTL_ADD, lfd, &e);
    fmt::print("[epoll-ET-{}] :{} (buf=128B)\n", g_fix ? "FIX" : "BUG", port);
    std::vector<epoll_event> evs(1024);
    while (true) {
        int n = ::epoll_wait(ep, evs.data(), evs.size(), -1);
        if (n < 0) { if (errno == EINTR) continue; die("epoll_wait"); }
        for (int i = 0; i < n; ++i) {
            int fd = evs[i].data.fd;
            if (fd == lfd) {
                while (true) {
                    int c = ::accept(lfd, nullptr, nullptr);
                    if (c < 0) { if (errno == EAGAIN) break; if (errno == EINTR) continue; die("accept"); }
                    set_nonblock(c);
                    epoll_event ce{}; ce.events = EPOLLIN | EPOLLET; ce.data.fd = c;
                    ::epoll_ctl(ep, EPOLL_CTL_ADD, c, &ce);
                }
            } else {
                g_fix ? handle_fix(fd) : handle_bug(fd);
            }
        }
    }
}
#endif
