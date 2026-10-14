// common.hpp — IO 盒公共工具
#pragma once
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdexcept>
#include <string>
#include "fmt_shim.hpp"

namespace mlab::io {

inline void die(const std::string& s) {
    throw std::runtime_error(s + ": " + std::strerror(errno));
}

inline void set_nonblock(int fd) {
    int fl = ::fcntl(fd, F_GETFL, 0);
    if (fl < 0 || ::fcntl(fd, F_SETFL, fl | O_NONBLOCK) < 0) die("fcntl");
}

inline int make_listener(int port, int backlog = 4096) {
    int fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) die("socket");
    int one = 1;
    ::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
    sockaddr_in a{};
    a.sin_family = AF_INET;
    a.sin_addr.s_addr = htonl(INADDR_ANY);
    a.sin_port = htons(port);
    if (::bind(fd, (sockaddr*)&a, sizeof(a)) < 0) die("bind");
    if (::listen(fd, backlog) < 0) die("listen");
    set_nonblock(fd);
    return fd;
}

inline ssize_t safe_read(int fd, char* buf, size_t n) {
    while (true) {
        ssize_t r = ::read(fd, buf, n);
        if (r >= 0) return r;
        if (errno == EINTR) continue;
        if (errno == EAGAIN || errno == EWOULDBLOCK) return -2;
        return -1;
    }
}

inline ssize_t safe_write(int fd, const char* buf, size_t n) {
    size_t sent = 0;
    while (sent < n) {
        ssize_t w = ::write(fd, buf + sent, n - sent);
        if (w < 0) {
            if (errno == EINTR) continue;
            if (errno == EAGAIN || errno == EWOULDBLOCK) return sent;
            return -1;
        }
        sent += w;
    }
    return sent;
}

inline void ignore_sigpipe() { ::signal(SIGPIPE, SIG_IGN); }

}  // namespace mlab::io
