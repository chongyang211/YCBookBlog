// stress_client — N 并发 × M 消息
#include "common.hpp"
#include <atomic>
#include <chrono>
#include <string>
#include <thread>
#include <vector>

using namespace mlab::io;
using Clock = std::chrono::steady_clock;

static struct Config {
    std::string host = "127.0.0.1";
    int port = 9001, conns = 10000, msgs = 10, size = 64;
} cfg;

static std::atomic<int>  g_done{0}, g_err{0};
static std::atomic<long> g_bytes{0};

static void one() {
    int fd = ::socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in a{}; a.sin_family = AF_INET; a.sin_port = htons(cfg.port);
    ::inet_pton(AF_INET, cfg.host.c_str(), &a.sin_addr);
    if (::connect(fd, (sockaddr*)&a, sizeof(a)) < 0) { g_err++; ::close(fd); return; }
    std::string pl(cfg.size, 'x'); char buf[4096];
    for (int i = 0; i < cfg.msgs; ++i) {
        if (safe_write(fd, pl.data(), pl.size()) != (ssize_t)pl.size()) { g_err++; break; }
        size_t got = 0;
        while (got < pl.size()) {
            ssize_t r = ::read(fd, buf, sizeof(buf));
            if (r <= 0) { g_err++; goto done; }
            got += r; g_bytes += r;
        }
    }
done:
    ::close(fd); g_done++;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        fmt::print("usage: {} <port> <conns> [msgs=10] [size=64]\n", argv[0]);
        return 1;
    }
    cfg.port  = std::atoi(argv[1]);
    cfg.conns = std::atoi(argv[2]);
    if (argc > 3) cfg.msgs = std::atoi(argv[3]);
    if (argc > 4) cfg.size = std::atoi(argv[4]);
    ignore_sigpipe();

    fmt::print("[stress] port={} conns={} msgs={} size={}B\n",
               cfg.port, cfg.conns, cfg.msgs, cfg.size);
    std::vector<std::thread> ts;
    ts.reserve(cfg.conns);
    auto t0 = Clock::now();
    for (int i = 0; i < cfg.conns; ++i) ts.emplace_back(one);
    for (auto& t : ts) t.join();
    double sec = std::chrono::duration<double>(Clock::now() - t0).count();
    long ops = (long)cfg.conns * cfg.msgs;
    fmt::print("done. {:.2f}s qps={:.0f} bw={:.1f}MB/s err={}\n",
               sec, ops / sec, g_bytes.load() / sec / 1024 / 1024, g_err.load());
    return 0;
}
