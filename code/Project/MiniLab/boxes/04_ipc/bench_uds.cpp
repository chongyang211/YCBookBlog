// bench_uds — Unix Domain Socket 带宽 + 乒乓延迟
#include "bench_common.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <string>

using namespace mlab::ipc;

static void bench_bandwidth(std::size_t MB) {
    int fds[2];
    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, fds) < 0) { perror("socketpair"); return; }
    std::size_t total = MB * 1024ULL * 1024ULL;
    std::size_t chunk = 64 * 1024;
    std::vector<char> buf(chunk, 'x');

    auto t0 = Clock::now();
    fork_run(
        [&]{
            ::close(fds[1]);
            std::size_t sent = 0;
            while (sent < total) {
                ssize_t w = ::write(fds[0], buf.data(),
                                    std::min(chunk, total - sent));
                if (w <= 0) break;
                sent += w;
            }
            ::close(fds[0]);
        },
        [&]{
            ::close(fds[0]);
            while (::read(fds[1], buf.data(), chunk) > 0) {}
            ::close(fds[1]);
        });
    double sec = std::chrono::duration<double>(Clock::now() - t0).count();
    fmt::print("uds bandwidth: {} MB in {:.2f}s = {:.2f} MB/s\n", MB, sec, MB / sec);
}

static void bench_latency(int N) {
    int fds[2];
    ::socketpair(AF_UNIX, SOCK_STREAM, 0, fds);
    fork_run(
        [&]{
            char c = 'x';
            auto t0 = Clock::now();
            for (int i = 0; i < N; ++i) {
                ::write(fds[0], &c, 1);
                ::read (fds[0], &c, 1);
            }
            double us = std::chrono::duration<double, std::micro>(
                Clock::now() - t0).count();
            fmt::print("uds pingpong: {} rtts in {:.1f}us = {:.2f} us/rtt\n",
                       N, us, us / N);
        },
        [&]{
            char c;
            for (int i = 0; i < N; ++i) {
                ::read (fds[1], &c, 1);
                ::write(fds[1], &c, 1);
            }
        });
}

int main(int argc, char** argv) {
    sync_stdio();
    std::string m = argc > 1 ? argv[1] : "all";
    if (m == "all" || m == "bw")  bench_bandwidth(1024);
    if (m == "all" || m == "lat") bench_latency(100000);
}
