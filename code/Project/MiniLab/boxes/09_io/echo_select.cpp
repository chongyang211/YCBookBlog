// echo_select — 传统 select 服务端
#include "common.hpp"
#include <sys/select.h>
#include <algorithm>
#include <vector>

using namespace mlab::io;

int main(int argc, char** argv) {
    int port = argc > 1 ? std::atoi(argv[1]) : 9001;
    ignore_sigpipe();
    int lfd = make_listener(port);
    std::vector<int> clients;
    fmt::print("[select] listening on :{} (FD_SETSIZE={})\n", port, FD_SETSIZE);
    while (true) {
        fd_set rfds; FD_ZERO(&rfds); FD_SET(lfd, &rfds);
        int maxfd = lfd;
        for (int c : clients) { FD_SET(c, &rfds); maxfd = std::max(maxfd, c); }
        int ready = ::select(maxfd + 1, &rfds, nullptr, nullptr, nullptr);
        if (ready < 0) { if (errno == EINTR) continue; die("select"); }
        if (FD_ISSET(lfd, &rfds)) {
            while (true) {
                int c = ::accept(lfd, nullptr, nullptr);
                if (c < 0) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) break;
                    if (errno == EINTR) continue;
                    die("accept");
                }
                if (c >= FD_SETSIZE) { ::close(c); continue; }
                set_nonblock(c);
                clients.push_back(c);
            }
        }
        for (auto it = clients.begin(); it != clients.end();) {
            if (!FD_ISSET(*it, &rfds)) { ++it; continue; }
            char buf[4096];
            ssize_t n = safe_read(*it, buf, sizeof(buf));
            if (n > 0) { safe_write(*it, buf, n); ++it; }
            else if (n == 0 || n == -1) { ::close(*it); it = clients.erase(it); }
            else { ++it; }
        }
    }
}
