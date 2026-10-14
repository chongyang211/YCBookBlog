// bench_shm — shm_open + sem_open 零拷贝带宽 + 延迟
#include "bench_common.hpp"
#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <vector>

using namespace mlab::ipc;

constexpr std::size_t SHM_SIZE = 1 << 20;

static void bench_bandwidth(std::size_t MB) {
    const char* shm = "/mlab_shm_bw";
    const char* r = "/mlab_shm_bw_r", *d = "/mlab_shm_bw_d";
    ::shm_unlink(shm); ::sem_unlink(r); ::sem_unlink(d);
    int fd = ::shm_open(shm, O_RDWR | O_CREAT, 0644);
    if (fd < 0) { perror("shm_open"); return; }
    ::ftruncate(fd, SHM_SIZE);
    void* buf = ::mmap(nullptr, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    sem_t* ready = ::sem_open(r, O_CREAT, 0644, 0);
    sem_t* done  = ::sem_open(d, O_CREAT, 0644, 0);

    std::size_t total = MB * 1024ULL * 1024ULL;
    std::vector<char> src(SHM_SIZE, 'x');

    auto t0 = Clock::now();
    fork_run(
        [&]{
            std::size_t sent = 0;
            while (sent < total) {
                std::size_t sz = std::min(SHM_SIZE, total - sent);
                std::memcpy(buf, src.data(), sz);
                ::sem_post(ready);
                ::sem_wait(done);
                sent += sz;
            }
        },
        [&]{
            std::vector<char> dst(SHM_SIZE);
            std::size_t got = 0;
            while (got < total) {
                ::sem_wait(ready);
                std::size_t sz = std::min(SHM_SIZE, total - got);
                std::memcpy(dst.data(), buf, sz);
                ::sem_post(done);
                got += sz;
            }
        });
    double sec = std::chrono::duration<double>(Clock::now() - t0).count();
    fmt::print("shm bandwidth: {} MB in {:.2f}s = {:.2f} MB/s\n", MB, sec, MB / sec);

    ::munmap(buf, SHM_SIZE); ::close(fd);
    ::shm_unlink(shm);
    ::sem_close(ready); ::sem_close(done);
    ::sem_unlink(r); ::sem_unlink(d);
}

static void bench_latency(int N) {
    const char* r = "/mlab_shm_lat_r", *d = "/mlab_shm_lat_d";
    ::sem_unlink(r); ::sem_unlink(d);
    sem_t* ready = ::sem_open(r, O_CREAT, 0644, 0);
    sem_t* done  = ::sem_open(d, O_CREAT, 0644, 0);
    auto t0 = Clock::now();
    fork_run(
        [&]{ for (int i = 0; i < N; ++i) { ::sem_post(ready); ::sem_wait(done); } },
        [&]{ for (int i = 0; i < N; ++i) { ::sem_wait(ready); ::sem_post(done); } });
    double us = std::chrono::duration<double, std::micro>(Clock::now() - t0).count();
    fmt::print("shm pingpong: {} rtts in {:.1f}us = {:.2f} us/rtt\n", N, us, us / N);
    ::sem_close(ready); ::sem_close(done);
    ::sem_unlink(r); ::sem_unlink(d);
}

int main(int argc, char** argv) {
    sync_stdio();
    std::string m = argc > 1 ? argv[1] : "all";
    if (m == "all" || m == "bw")  bench_bandwidth(1024);
    if (m == "all" || m == "lat") bench_latency(100000);
}
