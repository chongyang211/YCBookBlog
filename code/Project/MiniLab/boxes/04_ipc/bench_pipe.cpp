// bench_pipe — pipe 带宽 + 乒乓延迟
#include "bench_common.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <cstring>
#include <string>

using namespace mlab::ipc;

static void bench_bandwidth(std::size_t MB) {
    int fds[2]; if (::pipe(fds) < 0) { perror("pipe"); return; }
#ifdef __linux__
    ::fcntl(fds[1], F_SETPIPE_SZ, 1 << 20);
#endif
    std::size_t total = MB * 1024ULL * 1024ULL;
    std::size_t chunk = 64 * 1024;
    std::vector<char> buf(chunk, 'x');

    auto t0 = Clock::now();
    fork_run(
        [&]{
            ::close(fds[0]);
            std::size_t sent = 0;
            while (sent < total) {
                ssize_t w = ::write(fds[1], buf.data(),
                                    std::min(chunk, total - sent));
                if (w <= 0) break;
                sent += w;
            }
            ::close(fds[1]);
        },
        [&]{
            ::close(fds[1]);
            while (::read(fds[0], buf.data(), chunk) > 0) {}
            ::close(fds[0]);
        });
    double sec = std::chrono::duration<double>(Clock::now() - t0).count();
    fmt::print("pipe bandwidth: {} MB in {:.2f}s = {:.2f} MB/s\n",
               MB, sec, MB / sec);
}

static void bench_latency(int N) {
    int a2b[2], b2a[2]; ::pipe(a2b); ::pipe(b2a);
    fork_run(
        [&]{
            ::close(a2b[0]); ::close(b2a[1]);
            char c = 'x';
            auto t0 = Clock::now();
            for (int i = 0; i < N; ++i) {
                ::write(a2b[1], &c, 1);
                ::read (b2a[0], &c, 1);
            }
            double us = std::chrono::duration<double, std::micro>(
                Clock::now() - t0).count();
            fmt::print("pipe pingpong: {} rtts in {:.1f}us = {:.2f} us/rtt\n",
                       N, us, us / N);
            ::close(a2b[1]); ::close(b2a[0]);
        },
        [&]{
            ::close(a2b[1]); ::close(b2a[0]);
            char c;
            for (int i = 0; i < N; ++i) {
                ::read (a2b[0], &c, 1);
                ::write(b2a[1], &c, 1);
            }
            ::close(a2b[0]); ::close(b2a[1]);
        });
}

int main(int argc, char** argv) {
    sync_stdio();
    std::string m = argc > 1 ? argv[1] : "all";
    if (m == "all" || m == "bw")  bench_bandwidth(1024);
    if (m == "all" || m == "lat") bench_latency(100000);
}
