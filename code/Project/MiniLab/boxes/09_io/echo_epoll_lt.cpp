// echo_epoll_lt — Linux epoll 水平触发
#include "common.hpp"
#ifndef __linux__
int main() { fmt::print("epoll: Linux only\n"); return 0; }
#else
#include <sys/epoll.h>
#include <vector>

using namespace mlab::io;

int main(int argc, char** argv) {
    int port = argc > 1 ? std::atoi(argv[1]) : 9003;
    ignore_sigpipe();
    int lfd = make_listener(port);
    int ep = ::epoll_create1(EPOLL_CLOEXEC);
    if (ep < 0) die("epoll_create1");
    epoll_event e{}; e.events = EPOLLIN; e.data.fd = lfd;
    ::epoll_ctl(ep, EPOLL_CTL_ADD, lfd, &e);
    fmt::print("[epoll-LT] listening on :{}\n", port);
    std::vector<epoll_event> evs(1024);
    while (true) {
        int n = ::epoll_wait(ep, evs.data(), evs.size(), -1);
        if (n < 0) { if (errno == EINTR) continue; die("epoll_wait"); }
        for (int i = 0; i < n; ++i) {
            int fd = evs[i].data.fd;
            if (fd == lfd) {
                while (true) {
                    int c = ::accept(lfd, nullptr, nullptr);
                    if (c < 0) {
                        if (errno == EAGAIN) break;
                        if (errno == EINTR) continue;
                        die("accept");
                    }
                    set_nonblock(c);
                    epoll_event ce{}; ce.events = EPOLLIN; ce.data.fd = c;
                    ::epoll_ctl(ep, EPOLL_CTL_ADD, c, &ce);
                }
            } else {
                char buf[4096];
                ssize_t r = safe_read(fd, buf, sizeof(buf));
                if (r > 0) safe_write(fd, buf, r);
                else if (r == 0 || r == -1) {
                    ::epoll_ctl(ep, EPOLL_CTL_DEL, fd, nullptr);
                    ::close(fd);
                }
            }
        }
    }
}
#endif
