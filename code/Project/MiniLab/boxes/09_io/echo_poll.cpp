// echo_poll — poll 服务端
#include "common.hpp"
#include <poll.h>
#include <vector>

using namespace mlab::io;

int main(int argc, char** argv) {
    int port = argc > 1 ? std::atoi(argv[1]) : 9002;
    ignore_sigpipe();
    int lfd = make_listener(port);
    std::vector<pollfd> fds;
    fds.push_back({lfd, POLLIN, 0});
    fmt::print("[poll] listening on :{}\n", port);
    while (true) {
        int ready = ::poll(fds.data(), fds.size(), -1);
        if (ready < 0) { if (errno == EINTR) continue; die("poll"); }
        if (fds[0].revents & POLLIN) {
            while (true) {
                int c = ::accept(lfd, nullptr, nullptr);
                if (c < 0) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) break;
                    if (errno == EINTR) continue;
                    die("accept");
                }
                set_nonblock(c);
                fds.push_back({c, POLLIN, 0});
            }
        }
        for (int i = (int)fds.size() - 1; i >= 1; --i) {
            if (!(fds[i].revents & (POLLIN | POLLERR | POLLHUP))) continue;
            char buf[4096];
            ssize_t n = safe_read(fds[i].fd, buf, sizeof(buf));
            if (n > 0) safe_write(fds[i].fd, buf, n);
            else if (n == 0 || n == -1) {
                ::close(fds[i].fd); fds.erase(fds.begin() + i);
            }
        }
    }
}
