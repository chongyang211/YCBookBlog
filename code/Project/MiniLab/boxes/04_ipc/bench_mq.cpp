// bench_mq — POSIX 消息队列（Linux only）
#include "bench_common.hpp"
#ifdef __linux__
#include <fcntl.h>
#include <mqueue.h>
#include <cstring>
#include <string>
#include <vector>

using namespace mlab::ipc;

static void bench(int N, int msg_size) {
    const char* qn = "/mlab_mq_bench";
    mq_attr attr{};
    attr.mq_maxmsg  = 10;
    attr.mq_msgsize = msg_size;
    ::mq_unlink(qn);
    mqd_t mq = ::mq_open(qn, O_CREAT | O_RDWR, 0644, &attr);
    if (mq == (mqd_t)-1) { perror("mq_open"); return; }

    std::vector<char> payload(msg_size, 'x');
    std::vector<char> rbuf(msg_size);

    auto t0 = Clock::now();
    fork_run(
        [&]{ for (int i = 0; i < N; ++i)
                 if (::mq_send(mq, payload.data(), msg_size, 0) < 0)
                     { perror("send"); break; } },
        [&]{ mqd_t m2 = ::mq_open(qn, O_RDONLY);
             for (int i = 0; i < N; ++i)
                 if (::mq_receive(m2, rbuf.data(), msg_size, nullptr) < 0)
                     { perror("recv"); break; }
             ::mq_close(m2); });
    double sec = std::chrono::duration<double>(Clock::now() - t0).count();
    double mb  = (double)N * msg_size / 1024 / 1024;
    fmt::print("mq bandwidth: {} msgs × {} B = {:.2f} MB in {:.2f}s = {:.2f} MB/s\n",
               N, msg_size, mb, sec, mb / sec);
    ::mq_close(mq); ::mq_unlink(qn);
}

int main() { sync_stdio(); bench(200000, 4096); return 0; }
#else
int main() { fmt::print("bench_mq: Linux only\n"); return 0; }
#endif
